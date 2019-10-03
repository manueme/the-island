#pragma once
#include <string>
#include <assimp/camera.h>
#include <assimp/light.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <list>
#include <memory>

class GameObject;
class Shader;
class ShaderMaterialDefault;
class Light;
class Camera;
class Material;
class Mesh;
class Water;

using namespace std;

class SceneLoader {
public:
    SceneLoader();
    ~SceneLoader();
    shared_ptr<GameObject> loadScene(Assimp::Importer* importer,
                                     const string& filePath,
                                     list<shared_ptr<Light>>& illumination,
                                     vector<shared_ptr<Shader>>& shaders,
                                     vector<shared_ptr<Camera>>& cameras,
                                     list<shared_ptr<Water>>& waterObjects);
    shared_ptr<GameObject> loadScene(aiNode* node, const shared_ptr<GameObject>& parent);
    shared_ptr<GameObject> loadLight(aiLight* lightNode, const shared_ptr<GameObject>& parent) const;
    static shared_ptr<GameObject> loadCamera(aiCamera* cameraNode, const shared_ptr<GameObject>& parent);
    shared_ptr<Material> loadMaterial(aiMesh* meshNode, const shared_ptr<GameObject>& parent) const;

private:

    aiLight* getLightAssociated(aiNode* node) const;
    aiCamera* getCameraAssociated(aiNode* node) const;

    list<shared_ptr<Light>> auxIllumination;
    vector<shared_ptr<Shader>> auxShaders;
    vector<shared_ptr<Camera>> auxCameras;
    vector<shared_ptr<Water>> auxWaterObjects;
    const aiScene* auxScene;

    shared_ptr<ShaderMaterialDefault> materialDefaultShader;
    shared_ptr<Shader> waterShader;
    shared_ptr<Shader> skyBoxShader;
};
