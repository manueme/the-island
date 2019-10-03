#pragma once
#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform : public Component {
public:
    Transform(const shared_ptr<GameObject>& parent = nullptr);
    void setPosition(float x, float y, float z);
    void setPositionX(float x);
    void setPositionY(float y);
    void setPositionZ(float z);
    void setRotation(float x, float y, float z, float w);
    void setRotation(glm::quat newRotation);
    void setPosition(glm::vec3 newPosition);
    void setScale(glm::vec3 newScale);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void addRotation(glm::quat rotation);
    void setScale(float x, float y, float z);
    ComponentKey getComponentKey() override;
    glm::vec3 getPosition() const;
    glm::quat getRotation() const;
    glm::vec3 getScale() const;
    glm::vec3 getLocalPosition() const;
    glm::quat getLocalRotation() const;
    glm::vec3 getLocalScale() const;

    void setLocalPosition(glm::vec3 position);
    void setLocalRotation(glm::quat rotation);
    void setLocalScale(glm::vec3 scale);

    ~Transform();
private:
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(0);

};
