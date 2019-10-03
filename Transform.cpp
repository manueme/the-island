#include "Transform.h"
#include "GameObject.h"
#include "Mesh.h"

Transform::Transform(const shared_ptr<GameObject>& parent)
    : Component("transform", parent) {}

void Transform::setPosition(const float x, const float y, const float z)
{
    const glm::vec3 relative(x - position.x, y - position.y, z - position.z);
    const auto children = getParent()->getChildren();
    for (const auto& child : children) {
        auto childTransform = child->getTransform();
        const auto auxPos = childTransform->getPosition();
        childTransform->setPosition(auxPos.x + relative.x, auxPos.y + relative.y, auxPos.z + relative.z);
    }
    position = glm::vec3(x, y, z);
}

void Transform::setPositionX(const float x)
{
    const glm::vec3 relative(x - position.x, 0, 0);
    auto children = getParent()->getChildren();
    for (const auto& child : children) {
        auto childTransform = child->getTransform();
        const auto auxPos = childTransform->getPosition();
        childTransform->setPositionX(auxPos.x + relative.x);
    }
    position.x = x;
}

void Transform::setPositionY(const float y)
{
    const glm::vec3 relative(0, y - position.y, 0);
    auto children = getParent()->getChildren();
    for (const auto& child : children) {
        auto childTransform = child->getTransform();
        const auto auxPos = childTransform->getPosition();
        childTransform->setPositionY(auxPos.y + relative.y);
    }
    position.y = y;
}

void Transform::setPositionZ(const float z)
{
    const glm::vec3 relative(0, 0, z - position.z);
    auto children = getParent()->getChildren();
    for (const auto& child : children) {
        auto childTransform = child->getTransform();
        const auto auxPos = childTransform->getPosition();
        childTransform->setPositionZ(auxPos.z + relative.z);
    }
    position.z = z;
}

void Transform::setRotation(const float x, const float y, const float z, const float w)
{
    rotation.x = x;
    rotation.y = y;
    rotation.z = z;
    rotation.w = w;
    auto children = getParent()->getChildren();
    for (const auto& child : children) {
        auto childTransform = child->getTransform();
        childTransform->addRotation(rotation);
    }
}

void Transform::setRotation(const glm::quat newRotation)
{
    setRotation(newRotation.x, newRotation.y, newRotation.z, newRotation.w);
}

void Transform::setPosition(const glm::vec3 newPosition)
{
    setPosition(newPosition.x, newPosition.y, newPosition.z);
}

void Transform::setScale(const glm::vec3 newScale)
{
    setScale(newScale.x, newScale.y, newScale.z);
}

void Transform::rotateX(const float angle)
{
    rotation = glm::rotate(rotation, angle, glm::vec3(1.0f, 0.0f, 0.0f));
}

void Transform::rotateY(const float angle)
{
    rotation = glm::rotate(rotation, angle, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Transform::rotateZ(const float angle)
{
    rotation = glm::rotate(rotation, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Transform::addRotation(const glm::quat rotation)
{
    this->rotation *= rotation;
}

void Transform::setScale(const float x, const float y, const float z)
{
    scale = glm::vec3(x, y, z);
}

ComponentKey Transform::getComponentKey()
{
    return TransformComponent;
}

glm::vec3 Transform::getPosition() const
{
    return position;
}

glm::quat Transform::getRotation() const
{
    return rotation;
}

glm::vec3 Transform::getScale() const
{
    return scale;
}

glm::vec3 Transform::getLocalPosition() const
{
    const auto parentParent = parent->getParent();
    if (parentParent) {
        const auto parentTransform = parentParent->getTransform();
        const auto parentPosition = parentTransform->getPosition();
        return getPosition() - parentPosition;
    }
    return getPosition();
}

glm::quat Transform::getLocalRotation() const
{
    const auto parentParent = parent->getParent();
    if (parentParent) {
        const auto parentTransform = parentParent->getTransform();
        const auto parentRotation = parentTransform->getRotation();
        return getRotation() * inverse(parentRotation);
    }
    return getRotation();
}

glm::vec3 Transform::getLocalScale() const
{
    const auto parentParent = parent->getParent();
    if (parentParent) {
        const auto parentTransform = parentParent->getTransform();
        const auto parentScale = parentTransform->getScale();
        return getScale() * parentScale;
    }
    return getScale();
}

void Transform::setLocalPosition(const glm::vec3 position)
{
    const auto parentParent = parent->getParent();
    if (parentParent) {
        const auto parentTransform = parentParent->getTransform();
        const auto parentPosition = parentTransform->getPosition();
        setPosition(parentPosition + position);
    }
}

void Transform::setLocalRotation(const glm::quat rotation)
{
    const auto parentParent = parent->getParent();
    if (parentParent) {
        const auto parentTransform = parentParent->getTransform();
        const auto parentRotation = parentTransform->getRotation();
        setRotation(parentRotation * rotation);
    }
}

void Transform::setLocalScale(const glm::vec3 scale)
{
    const auto parentParent = parent->getParent();
    if (parentParent) {
        const auto parentTransform = parentParent->getTransform();
        const auto parentScale = parentTransform->getScale();
        setRotation(scale * glm::vec3(1.0 / parentScale.x, 1.0 / parentScale.y, 1.0 / parentScale.z));
    }
}

Transform::~Transform() = default;
