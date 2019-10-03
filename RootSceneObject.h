#pragma once
#include "GameObject.h"
#include <assimp/scene.h>

class RootSceneObject : public GameObject {
public:
    RootSceneObject(const std::string& name, const aiScene* scene);
    ~RootSceneObject();

    const aiScene* scene;
};
