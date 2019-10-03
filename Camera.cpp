#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const shared_ptr<GameObject>& parent)
    : Component("camera", parent)
{
    mainCamera = false;
}

void Camera::setLookAt(const float x, const float y, const float z)
{
    lookAt.x = x;
    lookAt.y = y;
    lookAt.z = z;
    cameraFront = normalize(getPos() - lookAt);
}

void Camera::setCameraFront(const float x, const float y, const float z)
{
    cameraFront.x = x;
    cameraFront.y = y;
    cameraFront.z = z;
}

void Camera::setCameraUp(const float x, const float y, const float z)
{
    cameraUp.x = x;
    cameraUp.y = y;
    cameraUp.z = z;
}

void Camera::setCameraRight(const float x, const float y, const float z)
{
    cameraRight.x = x;
    cameraRight.y = y;
    cameraRight.z = z;
}

ComponentKey Camera::getComponentKey()
{
    return CameraComponent;
}


glm::vec3 Camera::getPos() const
{
    return parentTransform->getPosition();
}

glm::vec3 Camera::getLookAt() const
{
    return lookAt;
}

glm::vec3 Camera::getCameraUp() const
{
    return cameraUp;
}

glm::vec3 Camera::getCameraRight() const
{
    return cameraRight;
}

glm::vec3 Camera::getCameraFront() const
{
    return cameraFront;
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(getPos(), getPos() + cameraFront, getCameraUp());
}

void Camera::objectMounted()
{
    parentTransform = getParent()->getTransform();
}

Camera::~Camera() = default;

void Camera::updateVectors()
{
    cameraFront = normalize(getPos() - lookAt);
    cameraRight = normalize(cross(cameraUp, cameraFront));
}
