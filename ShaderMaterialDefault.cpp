#include "ShaderMaterialDefault.h"
#include "OpenGLImports.h"
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Constants.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

ShaderMaterialDefault::ShaderMaterialDefault(const shared_ptr<GameObject>& parent, const shared_ptr<Material>& material)
    : Shader("DefaultMaterial.vert", "DefaultMaterial.frag", parent)
{
    this->material = material;
}

ShaderType ShaderMaterialDefault::getShaderType()
{
    return MaterialDefaultShader;
}


ShaderMaterialDefault::~ShaderMaterialDefault() = default;

void ShaderMaterialDefault::setClippingPlane(const glm::vec4 plane) const
{
    setVec4("clippingPlane", plane);
}


void ShaderMaterialDefault::setup(const shared_ptr<Material>& material, const shared_ptr<Mesh>& mesh) const
{
    if (!mesh) {
        return;
    }
    const auto meshParent = mesh->getParent();
    const auto transform = meshParent->getTransform();
    const auto currentCamera = meshParent->currentCamera;
    use();
    //******Camera Setup********
    setVec3("viewPos", currentCamera->getPos());
    const auto projection = glm::perspective(currentCamera->horizontalFOV, currentCamera->screenWidth / currentCamera->screenHeight, constants::NEAR_RENDER_PLANE, constants::FAR_RENDER_PLANE);
    const auto view = currentCamera->getViewMatrix();
    setMat4("projection", projection);
    setMat4("view", view);
    setMat4("model", glm::mat4());
    //**************************

    glm::mat4 model;
    model = translate(model, transform->getPosition());
    model *= mat4_cast(transform->getRotation());
    model = scale(model, transform->getScale());
    setMat4("model", model);
}

void ShaderMaterialDefault::setupLighting() const
{
    const auto illumination = parent->illumination;
    auto pointLightCount = 0;
    auto directionalLightCount = 0;
    auto spotLightCount = 0;
    use();
    for (const auto& light : illumination) {
        //******Light Setup*********
        const auto transform = light->getParent()->getTransform();
        switch (light->lType) {
        case LIGHT_AREA:
            break;
        case LIGHT_DIRECTIONAL: {
            // directional light
            auto posDirLight = std::to_string(directionalLightCount);
            setVec3("dirLights[" + posDirLight + "].direction", transform->getPosition());
            setVec3("dirLights[" + posDirLight + "].ambient", light->ambientColor);
            setVec3("dirLights[" + posDirLight + "].diffuse", light->diffuseColor);
            setVec3("dirLights[" + posDirLight + "].specular", light->specularColor);
            setFloat("dirLights[" + posDirLight + "].intensity", light->intensity);
            if (light->castShadows) {
                setMat4("shadowLightSpaceMatrix", light->matrixViewProjection);
                setInt("shadowMap", constants::SHADOW_MAP_GL_PLACE);
            }
            ++directionalLightCount;
        }
        break;
        case LIGHT_POINT: {
            auto posPointLight = std::to_string(pointLightCount);
            setVec3("pointLights[" + posPointLight + "].position", transform->getPosition());
            setVec3("pointLights[" + posPointLight + "].ambient", light->ambientColor);
            setVec3("pointLights[" + posPointLight + "].diffuse", light->diffuseColor);
            setVec3("pointLights[" + posPointLight + "].specular", light->specularColor);
            setFloat("pointLights[" + posPointLight + "].constant", light->constantAttenuation);
            setFloat("pointLights[" + posPointLight + "].linear", light->linearAttenuation);
            setFloat("pointLights[" + posPointLight + "].quadratic", light->quadraticAttenuation);
            setFloat("pointLights[" + posPointLight + "].intensity", light->intensity);
            ++pointLightCount;
        }
        break;
        case LIGHT_SPOT: {
            auto posSpotLight = std::to_string(spotLightCount);
            setVec3("spotLights[" + posSpotLight + "].position", transform->getPosition());
            setVec3("spotLights[" + posSpotLight + "].direction", eulerAngles(transform->getRotation()));
            setVec3("spotLights[" + posSpotLight + "].ambient", light->ambientColor);
            setVec3("spotLights[" + posSpotLight + "].diffuse", light->diffuseColor);
            setVec3("spotLights[" + posSpotLight + "].specular", light->specularColor);
            setFloat("spotLights[" + posSpotLight + "].constant", light->constantAttenuation);
            setFloat("spotLights[" + posSpotLight + "].linear", light->linearAttenuation);
            setFloat("spotLights[" + posSpotLight + "].quadratic", light->quadraticAttenuation);
            setFloat("spotLights[" + posSpotLight + "].cutOff", glm::cos(glm::radians(light->innerAngle)));
            setFloat("spotLights[" + posSpotLight + "].outerCutOff", glm::cos(glm::radians(light->outerAngle)));
            setFloat("spotLights[" + posSpotLight + "].intensity", light->intensity);
            ++spotLightCount;
        }
        break;
            //case lightType::LIGHT_VOLUME: resType = lightType::LIGHT_VOLUME; break;
        default:
            break;
        }
    }
    //Light counts
    setInt("pointLightCount", pointLightCount);
    setInt("spotLightCount", spotLightCount);
    setInt("directionalLightCount", directionalLightCount);
    //**************************
}

void ShaderMaterialDefault::setupMaterial() const
{
    if (material) {
        //*****Material Setup*******
        if (material->diffuseMap) {
            setInt("material.hasDiffuseMap", 1);
            setInt("material.diffuse", constants::GENERIC_MATERIAL_GL_PLACE);
            glActiveTexture(GL_TEXTURE0 + constants::GENERIC_MATERIAL_GL_PLACE);
            glBindTexture(GL_TEXTURE_2D, material->diffuseMap->getData());
        } else {
            glActiveTexture(GL_TEXTURE0 + constants::GENERIC_MATERIAL_GL_PLACE);
            glBindTexture(GL_TEXTURE_2D, 0);
            setInt("material.hasDiffuseMap", 0);
        }

        setVec4("material.diffuseColor", material->diffuse);
        setVec4("material.specularColor", material->specular);
        setVec4("material.ambientColor", material->ambient);
        setVec4("material.emissionColor", material->emission);
        setFloat("material.shininess", material->shininess);
        //**************************
    }
}

void ShaderMaterialDefault::objectMounted()
{
    setupLighting(); // STATIC LIGHTING, NO NEED TO UPDATE
    setupMaterial(); // ONLY ONE MATERIAL, NO NEED TO UPDATE, JUST INITIALIZE ONCE
}
