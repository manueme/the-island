#include "Light.h"
#include "OpenGLImports.h"
#include "GameObject.h"
#include "Constants.h"
#include "Transform.h"
#include "ShaderFastMeshRender.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Light::Light(lightType t, bool castShadows, const shared_ptr<GameObject>& parent)
    : Component("light", parent)
{
    this->castShadows = castShadows;
    if (this->castShadows) {
        shadowMapOpenGLBind = constants::SHADOW_MAP_GL_PLACE;
        setupShadowCasting();
    }

    diffuseColor.r = 1;
    diffuseColor.g = 1;
    diffuseColor.b = 1;

    specularColor.r = 1;
    specularColor.g = 1;
    specularColor.b = 1;

    ambientColor.r = 0;
    ambientColor.g = 0;
    ambientColor.b = 0;

    lType = t;
}

ComponentKey Light::getComponentKey()
{
    return LightComponent;
}

void Light::setupShadowMapping(const shared_ptr<ShaderFastMeshRender>& depthShader) const
{
    if (castShadows) {
        // 1. first render to depth map
        glViewport(0, 0, constants::SHADOW_MAPS_WIDTH, constants::SHADOW_MAPS_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        depthShader->use();
        depthShader->setMatrixViewProjection(matrixViewProjection);
        glCullFace(GL_FRONT);
    }
}

void Light::endShadowMapping() const
{
    if (castShadows) {
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0 + shadowMapOpenGLBind);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }
}

void Light::update() {}

void Light::objectMounted()
{
    parentTransform = parent->getTransform();
    const auto front = (parentTransform->getRotation() * glm::vec3(0.0f, -10.0f, 0.0f)) + parentTransform->getPosition();
    const auto lightView = lookAt(parentTransform->getPosition(),
                                  front,
                                  glm::vec3(0.0f, 1.0f, 0.0f));
    const auto lightProjection = glm::ortho(-1500.0f, 1500.0f, -1500.0f, 1500.0f, constants::NEAR_RENDER_PLANE, constants::FAR_RENDER_PLANE);
    this->matrixViewProjection = lightProjection * lightView;
}

void Light::setupShadowCasting()
{
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 constants::SHADOW_MAPS_WIDTH,
                 constants::SHADOW_MAPS_HEIGHT,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Light::~Light()
{
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);
}
