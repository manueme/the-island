#pragma once
#include <string>
#include "Constants.h"
#include <memory>

using namespace std;

class GameObject;
class ShaderFastMeshRender;

class Component : public enable_shared_from_this<Component> {
public:
    Component(const string& name, const shared_ptr<GameObject>& parent);
    void setParent(const shared_ptr<GameObject>& parent);
    shared_ptr<GameObject> getParent() const;
    string getName() const;
    virtual void shadowMappingRender(const shared_ptr<ShaderFastMeshRender>& depthShader);
    virtual void render();
    virtual void lateRender();
    virtual void update();
    virtual void objectMounted();
    virtual ComponentKey getComponentKey() = 0;
    virtual ~Component();
    bool enabled = true;
protected:
    string name;
    shared_ptr<GameObject> parent;
};
