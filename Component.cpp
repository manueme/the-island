#include "Component.h"
#include "GameObject.h"


Component::Component(const string& name, const shared_ptr<GameObject>& parent)
{
    this->name = name;
    this->parent = parent;
}

shared_ptr<GameObject> Component::getParent() const
{
    return parent;
}

void Component::setParent(const shared_ptr<GameObject>& parent)
{
    this->parent = parent;
}

string Component::getName() const
{
    return name;
}

void Component::shadowMappingRender(const shared_ptr<ShaderFastMeshRender>& depthShader) {}

void Component::update() {}

void Component::objectMounted() {}

void Component::render() {}

void Component::lateRender() {}

Component::~Component() = default;
