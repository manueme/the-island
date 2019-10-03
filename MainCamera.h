#pragma once
#include "GameObject.h"
#include <string>


class Camera;

class MainCamera final : public GameObject {
public:
    MainCamera(const string& name, const shared_ptr<GameObject>& mainScene);
    shared_ptr<Camera> getCamera() const;
    ~MainCamera();
    void createCamera();

private:
    void update() override;
    void keyboardKeyUp(KeyCode key) override;
    void keyboardKeyDown(KeyCode key) override;
    void mouseMotionEvent(int x, int y) override;
    void mouseButton(MouseButtonCode button, int xPos, int yPos) override;
    void onObjectMounted() override;
    shared_ptr<Camera> cam;

    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool raise = false;
    bool fall = false;

    bool rightClick = false;

    int lastMouseX = 400, lastMouseY = 300;

    float camSpeed = 0.0f;

};
