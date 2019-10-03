#include "MainCamera.h"
#include "Camera.h"
#include "Constants.h"
#include "Transform.h"
#include "glm/gtx/rotate_vector.hpp"
#include <glm/glm.hpp>

using namespace constants;

MainCamera::MainCamera(const string& name, const shared_ptr<GameObject>& mainScene)
    : GameObject(name, mainScene) {}

shared_ptr<Camera> MainCamera::getCamera() const
{
    return cam;
}

void MainCamera::update()
{
    auto t = getTransform();
    const auto front = normalize(glm::vec3(cam->getCameraFront().x, 0.0f, cam->getCameraFront().z));
    auto position = t->getPosition();
    const auto velocity = getSpeed() * 15.0f;
    if (forward) position += front * velocity;
    if (backward) position -= front * velocity;
    if (left) position -= normalize(cross(front, cam->getCameraUp())) * velocity;
    if (right) position += normalize(cross(front, cam->getCameraUp())) * velocity;
    if (raise) position += glm::vec3(0.0f, velocity, 0.0f);
    if (fall) position += glm::vec3(0.0f, -velocity, 0.0f);

    t->setPosition(position.x, position.y, position.z);
}


void MainCamera::keyboardKeyUp(const KeyCode key)
{
    if (key == SDLK_w || key == SDLK_UP) {
        forward = false;
        camSpeed = 0.0f;
    }
    if (key == SDLK_s || key == SDLK_DOWN) backward = false;
    if (key == SDLK_a || key == SDLK_LEFT) left = false;
    if (key == SDLK_d || key == SDLK_RIGHT) right = false;
    if (key == SDLK_SPACE) raise = false;
    if (key == SDLK_RCTRL || key == SDLK_LCTRL) fall = false;
}

void MainCamera::keyboardKeyDown(const KeyCode key)
{
    if (key == SDLK_w || key == SDLK_UP) forward = true;
    if (key == SDLK_s || key == SDLK_DOWN) backward = true;
    if (key == SDLK_a || key == SDLK_LEFT) left = true;
    if (key == SDLK_d || key == SDLK_RIGHT) right = true;
    if (key == SDLK_SPACE) raise = true;
    if (key == SDLK_RCTRL || key == SDLK_LCTRL) fall = true;
}

void MainCamera::mouseMotionEvent(const int x, const int y)
{
    if (rightClick) {
        auto xOffset = static_cast<float>(lastMouseX - x);
        auto yOffset = static_cast<float>(lastMouseY - y);
        lastMouseX = x;
        lastMouseY = y;
        const auto sensitivity = 0.01f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        auto front = glm::rotate(cam->getCameraFront(), xOffset, cam->getCameraUp());
        front = glm::rotate(front, yOffset, cam->getCameraRight());

        const auto cameraFront = normalize(front);
        const auto cameraRight = normalize(cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
        const auto cameraUp = normalize(cross(cameraRight, cameraFront));

        cam->setCameraFront(cameraFront.x, cameraFront.y, cameraFront.z);
        cam->setCameraRight(cameraRight.x, cameraRight.y, cameraRight.z);
        cam->setCameraUp(cameraUp.x, cameraUp.y, cameraUp.z);
    }
}

void MainCamera::mouseButton(const MouseButtonCode button, const int xPos, const int yPos)
{
    lastMouseX = xPos;
    lastMouseY = yPos;
    if (button == RightButton) {
        rightClick = !rightClick;
    }
}

void MainCamera::onObjectMounted() {}

MainCamera::~MainCamera() = default;

void MainCamera::createCamera()
{
    cam = make_shared<Camera>(shared_from_this());
    addComponent(cam);
}
