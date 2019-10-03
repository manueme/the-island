#pragma once
#include <list>
#include "Constants.h"
#include "Component.h"
#include <string>
#include <memory>
#include <SDL.h>
#include <vector>
using namespace std;

using KeyCode = SDL_Keycode;

enum MouseButtonCode {
    LeftButton = SDL_BUTTON_LEFT,
    MiddleButton = SDL_BUTTON_MIDDLE,
    RightButton = SDL_BUTTON_RIGHT,
    X1Button = SDL_BUTTON_X1,
    X2Button = SDL_BUTTON_X2
};

class Component;
class Transform;
class Light;
class Camera;
class Mesh;
class ShaderFastMeshRender;

class GameObject : public enable_shared_from_this<GameObject> {
public:
    explicit GameObject(const string& name, const shared_ptr<GameObject>& parent = nullptr);
    void setParent(const shared_ptr<GameObject>& parent);
    shared_ptr<GameObject> getParent() const;
    void addChild(const shared_ptr<GameObject>& child);
    shared_ptr<GameObject> findChild(const string& name);
    vector<shared_ptr<GameObject>> getChildren() const;
    list<shared_ptr<GameObject>> getGlobalChildrenList();
    shared_ptr<GameObject> findObject(const string& name);
    string getName() const;
    void addComponent(const shared_ptr<Component>& comp);

    shared_ptr<Component> getComponentFirst(ComponentKey key);

    list<shared_ptr<Component>> getComponentList(ComponentKey key);
    shared_ptr<Transform> getTransform() const;
    bool getTextureMode() const;
    void setName(const string& name);
    void setSpeed(float speed);
    float getSpeed() const;

    //Events
    void callUpdate();
    void callShadowMappingRender(const shared_ptr<ShaderFastMeshRender>& depthShader);
    void callRender();
    void callLateRender();
    void callMouseButton(MouseButtonCode button, int xPos, int yPos);
    void callMouseMotionEvent(int x, int y);
    void callKeyboardKeyUp(KeyCode key);
    void callKeyboardKeyDown(KeyCode key);
    void callOnCollisionEnter(const shared_ptr<GameObject>& other);
    void callOnCollisionStay(const shared_ptr<GameObject>& other);
    void callOnCollisionExit(const shared_ptr<GameObject>& other);
    void setIllumination(const list<shared_ptr<Light>>& illumination);
    void setCurrentCamera(const shared_ptr<Camera>& currentCamera);
    void callObjectMounted();
    //-------
    virtual ~GameObject();
    list<shared_ptr<Light>> illumination;
    shared_ptr<Camera> currentCamera;

private:
    virtual void update();
    virtual void render();
    virtual void mouseButton(MouseButtonCode button, int xPos, int yPos);
    virtual void mouseMotionEvent(int x, int y);
    virtual void keyboardKeyUp(KeyCode key);
    virtual void keyboardKeyDown(KeyCode key);
    virtual void onCollisionEnter(const shared_ptr<GameObject>& other);
    virtual void onCollisionStay(const shared_ptr<GameObject>& other);
    virtual void onCollisionExit(const shared_ptr<GameObject>& other);
    virtual void onObjectMounted();

    string name;
    vector<shared_ptr<Component>> components;
    string error;
    bool textureMode = true;
protected:
    vector<shared_ptr<GameObject>> children;
    shared_ptr<GameObject> parent;
    shared_ptr<Transform> transform;
    float speed = 1;
};
