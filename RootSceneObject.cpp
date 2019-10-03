#include "RootSceneObject.h"
#include <assimp/scene.h>
#include <assimp/cimport.h>

RootSceneObject::RootSceneObject(const std::string& name, const aiScene* scene)
    : GameObject(name)
{
    this->scene = scene;
}


RootSceneObject::~RootSceneObject()
{
    aiReleaseImport(scene);
    delete scene;
}
