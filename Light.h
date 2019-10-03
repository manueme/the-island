#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class ShaderFastMeshRender;
class Transform;

enum lightType {
    LIGHT_SPOT = 0,
    LIGHT_DIRECTIONAL = 1,
    LIGHT_AREA = 2,
    LIGHT_POINT = 3
};

class Light : public Component {
public:
    Light(lightType t, bool castShadows, const shared_ptr<GameObject>& parent = nullptr);
    ~Light();
    ComponentKey getComponentKey() override;
    glm::vec3 diffuseColor;
    glm::vec3 ambientColor;
    glm::vec3 specularColor;
    lightType lType;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float innerAngle;
    float outerAngle;
    float intensity;
    void setupShadowMapping(const shared_ptr<ShaderFastMeshRender>& depthShader) const;
    void endShadowMapping() const;
    void update() override;
    int shadowMapOpenGLBind;
    glm::mat4 matrixViewProjection;
    bool castShadows = false;
    void objectMounted() override;
private:
    //	For casting shadows:
    unsigned int depthMapFBO;
    unsigned int depthMap;
    void setupShadowCasting();
    shared_ptr<Transform> parentTransform;
    // **************
};
