#include "ShaderWater.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include <glm/gtc/matrix_transform.hpp>


ShaderWater::ShaderWater(const shared_ptr<GameObject>& parent)
    : Shader("WaterShader.vert", "WaterShader.frag", parent)
{
    moveFactor = 0;
    waterDistortionTexture = make_unique<Texture>("waterDuDvMap.png", true);
}

ComponentKey ShaderWater::getComponentKey()
{
    return ShaderComponent;
}

ShaderType ShaderWater::getShaderType()
{
    return WaterShader;
}

void ShaderWater::setup(Mesh* mesh) const
{
    use();
    const auto meshParent = mesh->getParent();
    const auto transform = meshParent->getTransform();
    const auto currentCamera = meshParent->currentCamera;
    const auto projection = glm::perspective(currentCamera->horizontalFOV, currentCamera->screenWidth / currentCamera->screenHeight, constants::NEAR_RENDER_PLANE, constants::FAR_RENDER_PLANE);
    const auto view = currentCamera->getViewMatrix();
    setMat4("matrixViewProjection", projection * view);
    glm::mat4 model;
    model = translate(model, transform->getPosition());
    model *= mat4_cast(transform->getRotation());
    model = scale(model, transform->getScale());
    setMat4("model", model);

    glActiveTexture(GL_TEXTURE0 + constants::WATER_DISTORTION_MAP_GL_PLACE);
    glBindTexture(GL_TEXTURE_2D, waterDistortionTexture->getData());
    setInt("waterDistortionMap", constants::WATER_DISTORTION_MAP_GL_PLACE);

    setFloat("moveFactor", static_cast<float>(moveFactor / 1200.0f));
    setVec3("viewPosition", currentCamera->getPos());
}

void ShaderWater::addReflectionTexture(const int texture) const
{
    setInt("reflectionTexture", texture);
}

void ShaderWater::addRefractionTexture(const int texture) const
{
    setInt("refractionTexture", texture);
}

void ShaderWater::update()
{
    moveFactor += 1;
    moveFactor %= 1200;
}


ShaderWater::~ShaderWater() = default;
