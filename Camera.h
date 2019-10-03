#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class GameObject;
class Transform;

class Camera final : public Component {
public:
    explicit Camera(const shared_ptr<GameObject>& parent);
    ~Camera();
    void updateVectors();
    ComponentKey getComponentKey() override;
    void setLookAt(float x, float y, float z);
    void setCameraFront(float x, float y, float z);
    void setCameraUp(float x, float y, float z);
    void setCameraRight(float x, float y, float z);
    glm::vec3 getPos() const;
    glm::vec3 getLookAt() const;
    glm::vec3 getCameraUp() const;
    glm::vec3 getCameraRight() const;
    glm::vec3 getCameraFront() const;
    glm::mat4 getViewMatrix() const;

    void objectMounted() override;

    bool mainCamera;
    float horizontalFOV;
    float screenWidth;
    float screenHeight;

private:
    glm::vec3 lookAt;
    glm::vec3 cameraUp;
    glm::vec3 cameraFront;
    glm::vec3 cameraRight;
    shared_ptr<Transform> parentTransform;
};
