#version 330 core
layout(location = 0) out vec4 color;

// Initial version from: https://learnopengl.com/Lighting/Light-casters

struct Material {
	int hasDiffuseMap;
    sampler2D diffuse;
	vec4 diffuseColor;
	vec4 specularColor;
    float shininess;
	vec4 ambientColor;
	vec4 emissionColor;
}; 

struct DirLight {
	float intensity;

    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
	float intensity;

    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
	float intensity;

    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_LIGHT_COUNT 40

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosShadowLightSpace;
} fs_in;

uniform int pointLightCount;
uniform int spotLightCount;
uniform int directionalLightCount;

uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform DirLight dirLights[MAX_LIGHT_COUNT];
uniform PointLight pointLights[MAX_LIGHT_COUNT];
uniform SpotLight spotLights[MAX_LIGHT_COUNT];
uniform Material material;

// function prototypes
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
	vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
	for(int i = 0; i < directionalLightCount && i < MAX_LIGHT_COUNT; i++)
		result += CalcDirLight(dirLights[i], norm, viewDir);
    // phase 2: point lights
    for(int j = 0; j < pointLightCount && j < MAX_LIGHT_COUNT; j++)
        result += CalcPointLight(pointLights[j], norm, fs_in.FragPos, viewDir);
    // phase 3: spot light
	for(int k = 0; k < spotLightCount && k < MAX_LIGHT_COUNT; k++)
		result += CalcSpotLight(spotLights[k], norm, fs_in.FragPos, viewDir);
    color = result + material.emissionColor;
	//FragColor = vec4(1.0, 0.0, 0.0, 0.1);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	if(projCoords.z > 1.0) {
		return 0.0;
	}
    // check whether current frag pos is in shadow
	float bias = 0.0009;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -3; x <= 3; ++x)
	{
		for(int y = -3; y <= 3; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 49.0;
    return shadow;
} 

// calculates the color when using a directional light.
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec4 ambient = vec4(light.ambient, 1.0) + vec4(0.3, 0.3, 0.3, 0.0);
	vec4 diffuse = vec4(light.diffuse, 1.0);
	vec4 specular = vec4(light.specular, 1.0);
	if (material.hasDiffuseMap > 0) {
		ambient = ambient * vec4(texture(material.diffuse, fs_in.TexCoords)) * material.diffuseColor;
		diffuse = diffuse * diff * vec4(texture(material.diffuse, fs_in.TexCoords)) * material.diffuseColor;
	}
	else {
		ambient = ambient * material.ambientColor;
		diffuse = diffuse * diff * material.diffuseColor;
	}
	specular = specular * spec * material.specularColor;

	// calculate shadow
	float shadow = 0.0;
	if (material.diffuseColor.w >= 1.0) {
	    shadow = ShadowCalculation(fs_in.FragPosShadowLightSpace);
	}
	vec4 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * light.intensity;
	if (result.w > 1) result.w = 1;
    return result;
}

// calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
	vec4 ambient = vec4(light.ambient, 1.0);
	vec4 diffuse = vec4(light.diffuse, 1.0);
	vec4 specular = vec4(light.specular, 1.0);
	if (material.hasDiffuseMap > 0) {
		ambient = ambient * texture(material.diffuse, fs_in.TexCoords) * material.ambientColor;
		diffuse = diffuse * diff * texture(material.diffuse, fs_in.TexCoords) * material.diffuseColor;
	}
	else {
		ambient = ambient * material.diffuseColor;
		diffuse = diffuse * diff * material.diffuseColor;
	}
	specular = specular * spec * material.specularColor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular) * light.intensity;
}

// calculates the color when using a spot light.
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
	vec4 ambient = vec4(light.ambient, 1.0);
	vec4 diffuse = vec4(light.diffuse, 1.0);
	vec4 specular = vec4(light.specular, 1.0);
	if (material.hasDiffuseMap > 0) {
		ambient = ambient * vec4(texture(material.diffuse, fs_in.TexCoords)) * material.ambientColor;
		diffuse = diffuse * diff * vec4(texture(material.diffuse, fs_in.TexCoords)) * material.diffuseColor;
	}
	else {
		ambient = ambient * material.diffuseColor;
		diffuse = diffuse * diff * material.diffuseColor;
	}
	specular = specular * spec * material.specularColor;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular) * light.intensity;
}