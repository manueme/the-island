#include "MainWindow.h"
#include "SceneLoader.h"
#include "GameObject.h"
#include "Light.h"
#include "ShaderFastMeshRender.h"
#include "Camera.h"
#include "Water.h"
#include "ShaderWater.h"
#include "ShaderMaterialDefault.h"
#include "Transform.h"
#include <cstdio>
#include <GL/glew.h>
#include <GL/GLU.h>

void MainWindow::initSceneAndShaders()
{
    SceneLoader sceneLoader;
    scene = sceneLoader.loadScene(&importer, "DemoScene.fbx", illumination, shaders, cameras, waterObjects);
    scene->setIllumination(illumination);
    shared_ptr<Camera> mainCamera = nullptr;
    for (const auto& camera : cameras) {
        if (camera->mainCamera) {
            mainCamera = camera;
        }
    }
    if ((mainCamera == nullptr) && !cameras.empty()) {
        mainCamera = cameras.at(0);
    }
    scene->setCurrentCamera(mainCamera);
    depthShader = std::make_shared<ShaderFastMeshRender>(scene);
    scene->addComponent(depthShader);
}

MainWindow::MainWindow() = default;

MainWindow::~MainWindow()
{
    SDL_DestroyWindow(sdlWindow);
    sdlWindow = nullptr;
    SDL_Quit();
}

bool MainWindow::show()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    sdlWindow = SDL_CreateWindow("Boilerplate: Assimp-OpenGL-SDL",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 constants::SCREEN_WIDTH,
                                 constants::SCREEN_HEIGHT,
                                 SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (sdlWindow == nullptr) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    sdlContext = SDL_GL_CreateContext(sdlWindow);
    if (sdlContext == nullptr) {
        printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    glewExperimental = GL_TRUE;
    const auto glewError = glewInit();
    if (glewError != GLEW_OK) {
        printf("Error initializing GLEW! %p\n", glewGetErrorString(glewError));
    }

    initSceneAndShaders();
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.f, 0.f, 0.f, 1.f);

    const auto error = glGetError();
    if (error != GL_NO_ERROR) {
        printf("Error initializing OpenGL!");
        return false;
    }

    scene->callObjectMounted();

    return true;
}

void MainWindow::propagateUpdate() const
{
    scene->callUpdate();
}

void MainWindow::propagateRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& light : illumination) {
        if (light->castShadows) {
            light->setupShadowMapping(depthShader);
            scene->callShadowMappingRender(depthShader);
            light->endShadowMapping();

            break; // TODO: Support more than one shadow light
        }
    }

    glEnable(GL_CLIP_DISTANCE0);
    for (const auto& waterObject : waterObjects) {
        auto clippingPlane = glm::vec4(0.0, -1.0, 0.0, waterObject->getTransform()->getPosition().y);
        auto shader = static_pointer_cast<ShaderMaterialDefault>(shaders.at(0));
        shader->use();
        shader->setClippingPlane(clippingPlane);

        waterObject->setupRefraction();
        scene->callRender();
        waterObject->endRefraction();

        //Camera Mirror
        auto cam = scene->currentCamera;

        auto cameraTransform = cam->getParent()->getTransform();
        auto auxPosition = cameraTransform->getPosition();
        auto auxFront = cam->getCameraFront();
        auto auxRight = cam->getCameraRight();
        auto auxUp = cam->getCameraUp();

        cameraTransform->setPositionY(auxPosition.y - 2 * (auxPosition.y - waterObject->getTransform()->getPosition().y));
        auto newCameraFront = normalize(glm::vec3(auxFront.x, auxFront.y * -1.0, auxFront.z));
        auto newCameraRight = normalize(cross(newCameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
        auto newCameraUp = normalize(cross(newCameraRight, newCameraFront));

        cam->setCameraFront(newCameraFront.x, newCameraFront.y, newCameraFront.z);
        cam->setCameraRight(newCameraRight.x, newCameraRight.y, newCameraRight.z);
        cam->setCameraUp(newCameraUp.x, newCameraUp.y, newCameraUp.z);

        shader->use();
        clippingPlane = glm::vec4(0.0, 1.0, 0.0, -waterObject->getTransform()->getPosition().y);
        shader->setClippingPlane(clippingPlane);

        waterObject->setupReflection();
        scene->callRender();
        scene->callLateRender();
        waterObject->endReflection();

        // Camera Reset
        cameraTransform->setPosition(auxPosition);
        cam->setCameraFront(auxFront.x, auxFront.y, auxFront.z);
        cam->setCameraRight(auxRight.x, auxRight.y, auxRight.z);
        cam->setCameraUp(auxUp.x, auxUp.y, auxUp.z);
    }
    glDisable(GL_CLIP_DISTANCE0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene->callRender();
    scene->callLateRender();

    auto shader = static_pointer_cast<ShaderWater>(shaders.at(1));
    for (const auto& waterObject : waterObjects) {
        shader->use();
        shader->addReflectionTexture(waterObject->bindReflectionTexture());
        shader->addRefractionTexture(waterObject->bindRefractionTexture());
        waterObject->renderWater();
    }

    SDL_GL_SwapWindow(sdlWindow);
}

void MainWindow::propagateKeyPressed(const KeyCode key) const
{
    scene->callKeyboardKeyDown(key);
}

void MainWindow::propagateKeyUp(const KeyCode key) const
{
    scene->callKeyboardKeyUp(key);
}

void MainWindow::propagateMouse(const MouseButtonCode button, const int x, const int y) const
{
    scene->callMouseButton(button, x, y);
}

void MainWindow::propagateMouseMoved(const int x, const int y) const
{
    scene->callMouseMotionEvent(x, y);
}
