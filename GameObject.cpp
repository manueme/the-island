#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

GameObject::GameObject(const string& name, const shared_ptr<GameObject>& parent)
{
    this->name = name;
    this->parent = parent;
}

void GameObject::setParent(const shared_ptr<GameObject>& parent)
{
    this->parent = parent;
}

shared_ptr<GameObject> GameObject::getParent() const
{
    return parent;
}

void GameObject::addChild(const shared_ptr<GameObject>& child)
{
    children.push_back(child);
}

shared_ptr<GameObject> GameObject::findChild(const string& name)
{
    const auto it = find_if(children.begin(),
                            children.end(),
                            [name](const shared_ptr<GameObject>& child) { return child->getName() == name; });
    if (it != children.end()) {
        return *it;
    }
    for (const auto& child : children) {
        const auto result = child->findChild(name);
        if (result) {
            return result;
        }
    }
    return nullptr;
}

vector<shared_ptr<GameObject>> GameObject::getChildren() const
{
    return children;
}

list<shared_ptr<GameObject>> GameObject::getGlobalChildrenList()
{
    list<shared_ptr<GameObject>> result;
    for (const auto& child : children) {
        result.push_back(child);
        result.splice(result.end(), child->getGlobalChildrenList());
    }
    return result;
}

shared_ptr<GameObject> GameObject::findObject(const string& name)
{
    auto root = shared_from_this();
    auto auxRoot = root;
    while (auxRoot) {
        auxRoot = root->getParent();
        if (auxRoot) root = auxRoot;
    }
    if (root->getName() == name) return root;
    return root->findChild(name);
}

string GameObject::getName() const
{
    return name;
}

void GameObject::setName(const string& name)
{
    this->name = name;
}

void GameObject::addComponent(const shared_ptr<Component>& comp)
{
    if (comp->getComponentKey() == TransformComponent) {
        this->transform = static_pointer_cast<Transform>(comp);
    }
    components.push_back(comp);
}

shared_ptr<Component> GameObject::getComponentFirst(ComponentKey key)
{
    const auto it = find_if(components.begin(),
                            components.end(),
                            [key](const shared_ptr<Component>& c) { return c->getComponentKey() == key; });
    return it == components.end() ? nullptr : *it;
}

void GameObject::setSpeed(const float speed)
{
    this->speed = speed;
    for (const auto& child : children) {
        child->setSpeed(speed);
    }
}

float GameObject::getSpeed() const
{
    return speed;
}

void GameObject::callUpdate()
{
    for (const auto& child : children) {
        child->callUpdate();
    }
    for (const auto& component : components) {
        component->update();
    }
    update();
}

void GameObject::callShadowMappingRender(const shared_ptr<ShaderFastMeshRender>& depthShader)
{
    for (const auto& component : components) {
        component->shadowMappingRender(depthShader);
    }
    for (const auto& child : children) {
        child->callShadowMappingRender(depthShader);
    }
}

void GameObject::callRender()
{
    for (const auto& component : components) {
        component->render();
    }
    render();
    for (const auto& child : children) {
        child->callRender();
    }
}

void GameObject::callLateRender()
{
    for (const auto& component : components) {
        component->lateRender();
    }
    for (const auto& child : children) {
        child->callLateRender();
    }
}

void GameObject::callMouseButton(const MouseButtonCode button, int xPos, int yPos)
{
    for (const auto& child : children) {
        child->callMouseButton(button, xPos, yPos);
    }
    mouseButton(button, xPos, yPos);
}

void GameObject::callKeyboardKeyUp(const KeyCode key)
{
    for (const auto& child : children) {
        child->callKeyboardKeyUp(key);
    }
    keyboardKeyUp(key);
}

void GameObject::callKeyboardKeyDown(const KeyCode key)
{
    for (const auto& child : children) {
        child->callKeyboardKeyDown(key);
    }
    keyboardKeyDown(key);
}

void GameObject::callMouseMotionEvent(const int x, const int y)
{
    for (const auto& child : children) {
        child->callMouseMotionEvent(x, y);
    }
    mouseMotionEvent(x, y);
}

void GameObject::callOnCollisionEnter(const shared_ptr<GameObject>& other)
{
    onCollisionEnter(other);
}

void GameObject::callOnCollisionStay(const shared_ptr<GameObject>& other)
{
    onCollisionStay(other);
}

void GameObject::callOnCollisionExit(const shared_ptr<GameObject>& other)
{
    onCollisionExit(other);
}

void GameObject::setIllumination(const list<shared_ptr<Light>>& illumination)
{
    for (const auto& child : children) {
        child->setIllumination(illumination);
    }
    this->illumination = illumination;
}

void GameObject::setCurrentCamera(const shared_ptr<Camera>& currentCamera)
{
    for (const auto& child : children) {
        child->setCurrentCamera(currentCamera);
    }
    this->currentCamera = currentCamera;
}

void GameObject::callObjectMounted()
{
    for (const auto& component : components) {
        component->objectMounted();
    }
    onObjectMounted();
    for (const auto& child : children) {
        child->callObjectMounted();
    }
}

void GameObject::update() {}

void GameObject::render() {}

void GameObject::mouseButton(const MouseButtonCode button, int xPos, int yPos) {}

void GameObject::keyboardKeyUp(const KeyCode key) {}

void GameObject::keyboardKeyDown(const KeyCode key) {}

void GameObject::mouseMotionEvent(int x, int y) {}

void GameObject::onCollisionEnter(const shared_ptr<GameObject>& other) {}

void GameObject::onCollisionStay(const shared_ptr<GameObject>& other) {}

void GameObject::onCollisionExit(const shared_ptr<GameObject>& other) {}

void GameObject::onObjectMounted() {}

list<shared_ptr<Component>> GameObject::getComponentList(ComponentKey key)
{
    list<shared_ptr<Component>> result;
    for (const auto& component : components) {
        if (component->getComponentKey() == key) {
            result.push_back(component);
        }
    }
    return result;
}

bool GameObject::getTextureMode() const
{
    return textureMode;
}

shared_ptr<Transform> GameObject::getTransform() const
{
    return transform;
}

GameObject::~GameObject() = default;
