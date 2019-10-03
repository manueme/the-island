#pragma once
#include <vector>
#include <SDL.h>
#include "GameObject.h"
#include <assimp/Importer.hpp>

class Shader;
class ShaderFastMeshRender;
class Water;

class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    void initSceneAndShaders();
    bool show();

    void propagateUpdate() const;
    void propagateRender();
    void propagateKeyPressed(KeyCode key) const;
    void propagateKeyUp(KeyCode key) const;
    void propagateMouse(MouseButtonCode button, int x, int y) const;
    void propagateMouseMoved(int x, int y) const;

private:
    shared_ptr<GameObject> scene;
    SDL_Window* sdlWindow;
    SDL_GLContext sdlContext;
    std::list<shared_ptr<Light>> illumination;
    std::vector<shared_ptr<Shader>> shaders;
    std::vector<shared_ptr<Camera>> cameras;
    std::list<shared_ptr<Water>> waterObjects;
    shared_ptr<ShaderFastMeshRender> depthShader;
    Assimp::Importer importer;
};
