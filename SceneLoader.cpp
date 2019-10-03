#include "SceneLoader.h"
#include "Camera.h"
#include "Constants.h"
#include "Light.h"
#include "MainCamera.h"
#include "Material.h"
#include "Mesh.h"
#include "ObjectAnimation.h"
#include "RootSceneObject.h"
#include "ShaderMaterialDefault.h"
#include "ShaderMaterialSkyBox.h"
#include "ShaderWater.h"
#include "SkyBox.h"
#include "Texture.h"
#include "Transform.h"
#include "Water.h"
#include <assimp/postprocess.h> // Post processing flags
#include <glm/gtc/quaternion.hpp>

SceneLoader::SceneLoader()
{
    materialDefaultShader = make_shared<ShaderMaterialDefault>(nullptr, nullptr);
    auxShaders.push_back(materialDefaultShader);
    waterShader = make_shared<ShaderWater>(nullptr);
    auxShaders.push_back(waterShader);
    skyBoxShader = make_shared<ShaderMaterialSkyBox>(nullptr);
    auxShaders.push_back(skyBoxShader);
}


SceneLoader::~SceneLoader()
= default;

shared_ptr<GameObject> SceneLoader::loadScene(Assimp::Importer* importer,
                                              const string& filePath,
                                              list<shared_ptr<Light>>& illumination,
                                              vector<shared_ptr<Shader>>& shaders,
                                              vector<shared_ptr<Camera>>& cameras,
                                              list<shared_ptr<Water>>& waterObjects)
{
    auxScene = importer->ReadFile(filePath,
                                  aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
    if (auxScene == nullptr) {
        printf(importer->GetErrorString());
        return nullptr;
    }

    const auto rootNode = auxScene->mRootNode;

    auto scene = loadScene(rootNode, nullptr); //Recursive load

    for (const auto& shader : auxShaders) {
        if (shader->getParent() == nullptr) {
            shader->setParent(scene);
        }
        shaders.push_back(shader);
    }
    auxShaders.clear();
    for (const auto& light : auxIllumination) {
        illumination.push_back(light);
    }
    auxIllumination.clear();
    for (const auto& camera : auxCameras) {
        cameras.push_back(camera);
    }
    auxCameras.clear();
    for (const auto& water : auxWaterObjects) {
        waterObjects.push_back(water);
    }
    auxWaterObjects.clear();
    return scene;
}

shared_ptr<GameObject> SceneLoader::loadScene(aiNode* node, const shared_ptr<GameObject>& parent)
{
    if (node != nullptr) {
        shared_ptr<GameObject> res = nullptr;
        auto genericNode = true;
        const auto associatedLight = getLightAssociated(node);
        if (associatedLight != nullptr) {
            genericNode = false;

            res = loadLight(associatedLight, parent);
            auxIllumination.push_back(static_pointer_cast<Light>(res->getComponentFirst(LightComponent)));
        }
        const auto associatedCamera = getCameraAssociated(node);
        if (associatedCamera != nullptr) {
            genericNode = false;

            res = loadCamera(associatedCamera, parent);
            auxCameras.push_back(static_pointer_cast<Camera>(res->getComponentFirst(CameraComponent)));
        }
        if (node->mNumMeshes > 0) {
            genericNode = false;
            auto doNotRender = false;
            // WATER OBJECT
            if (node->mName == aiString("WATER")) {
                auto water = make_shared<Water>(node->mName.C_Str(), parent);
                water->addComponent(waterShader);
                res = water;
                waterShader->setParent(res);
                auxWaterObjects.push_back(water);
                doNotRender = true;
            } else {
                res = make_shared<GameObject>(node->mName.C_Str(), parent);
                res->addComponent(materialDefaultShader);
            }

            for (auto i = 0u; i < node->mNumMeshes; ++i) {
                auto mesh = auxScene->mMeshes[node->mMeshes[i]];
                auto meshComponent = make_shared<Mesh>(mesh, res);
                res->addComponent(meshComponent);
                if (!materialDefaultShader->material) {
                    // yes, we have only one material for all the scene
                    auto mat = loadMaterial(mesh, res);
                    if (mat != nullptr) {
                        meshComponent->material = mat;
                        materialDefaultShader->material = mat;
                    }
                }
                meshComponent->doNotRender = doNotRender;
            }
        }

        if (genericNode && (parent != nullptr)) {
            res = make_shared<GameObject>(node->mName.C_Str(), parent);
        } else if (parent == nullptr) {
            // Root Node
            res = make_shared<RootSceneObject>(node->mName.C_Str(), auxScene);

            // Root Node generates the Skybox
            vector<string> skyboxFaces{
                "skybox/cloudtop_rt.tga",
                "skybox/cloudtop_lf.tga",
                "skybox/cloudtop_up.tga",
                "skybox/cloudtop_dn.tga",
                "skybox/cloudtop_bk.tga",
                "skybox/cloudtop_ft.tga"
            };
            const auto skyBox = make_shared<SkyBox>(skyboxFaces, res);
            res->addComponent(skyBoxShader);
            res->addComponent(skyBox);
        }

        if (res != nullptr) {
            // Animation loading
            for (auto i = 0u; i < auxScene->mNumAnimations; ++i) {
                auto anim = auxScene->mAnimations[i];
                for (auto j = 0u; j < anim->mNumChannels; ++j) {
                    auto channel = anim->mChannels[j];
                    if (channel->mNodeName == node->mName) {
                        auto compAnim = make_shared<ObjectAnimation>(channel, anim->mDuration, anim->mTicksPerSecond, res);
                        res->addComponent(compAnim);
                        // compAnim->pauseAnimation();
                    }
                }
            }

            // *****************

            for (auto i = 0u; i < node->mNumChildren; ++i) {
                auto child = loadScene(node->mChildren[i], res);
                if (child != nullptr) {
                    res->addChild(child);
                }
            }

            aiVector3D position;
            aiQuaternion rotation;
            aiVector3D scale;
            node->mTransformation.Decompose(scale, rotation, position);
            auto transform = res->getTransform();
            if (!transform) {
                transform = std::make_shared<Transform>(res);
                res->addComponent(transform);
            }
            transform->setPosition(position.x, position.y, position.z);
            transform->setScale(scale.x, scale.y, scale.z);
            transform->setRotation(rotation.x, rotation.y, rotation.z, rotation.w);
            auto camera = static_pointer_cast<Camera>(res->getComponentFirst(CameraComponent));
            if (camera != nullptr) {
                auto front = (transform->getRotation() * glm::vec3(0.0f, 1.0f, 0.0f)) - transform->getPosition();
                auto right = normalize(cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
                auto up = normalize(cross(right, front));
                camera->setCameraFront(front.x, front.y, front.z);
                camera->setCameraRight(right.x, right.y, right.z);
                camera->setCameraUp(up.x, up.y, up.z);
            }
            return res;
        }
    }
    return nullptr;
}

shared_ptr<GameObject> SceneLoader::loadLight(aiLight* lightNode, const shared_ptr<GameObject>& parent) const
{
    const auto lType = lightNode->mType;
    lightType resType;
    switch (lType) {
    case aiLightSource_AREA:
        resType = LIGHT_AREA;
        break;
    case aiLightSource_DIRECTIONAL:
        resType = LIGHT_DIRECTIONAL;
        break;
    case aiLightSource_POINT:
        resType = LIGHT_POINT;
        break;
    case aiLightSource_SPOT:
        resType = LIGHT_SPOT;
        break;
    default:
        resType = LIGHT_POINT;
        break;
    }
    auto objectLight = make_shared<GameObject>(lightNode->mName.C_Str(), parent);
    const auto light = make_shared<Light>(resType, resType == LIGHT_DIRECTIONAL, objectLight);

    light->diffuseColor.r = lightNode->mColorDiffuse.r;
    light->diffuseColor.g = lightNode->mColorDiffuse.g;
    light->diffuseColor.b = lightNode->mColorDiffuse.b;
    light->ambientColor.r = lightNode->mColorAmbient.r;
    light->ambientColor.g = lightNode->mColorAmbient.g;
    light->ambientColor.b = lightNode->mColorAmbient.b;
    light->specularColor.r = lightNode->mColorSpecular.r;
    light->specularColor.g = lightNode->mColorSpecular.g;
    light->specularColor.b = lightNode->mColorSpecular.b;
    //Attenuation
    light->constantAttenuation = lightNode->mAttenuationConstant / 1000.0f;
    light->linearAttenuation = lightNode->mAttenuationLinear / 1000.0f;
    light->quadraticAttenuation = lightNode->mAttenuationQuadratic / 1000.0f;
    //SpotLight Angles
    light->innerAngle = lightNode->mAngleInnerCone;
    light->outerAngle = lightNode->mAngleOuterCone;

    //Intencity
    light->intensity = 1.0f;

    objectLight->addComponent(light);
    return objectLight;
}

shared_ptr<GameObject> SceneLoader::loadCamera(aiCamera* cameraNode, const shared_ptr<GameObject>& parent)
{
    const auto mainCamera = make_shared<MainCamera>(cameraNode->mName.C_Str(), parent);
    mainCamera->createCamera();
    const auto camera = mainCamera->getCamera();
    if (camera->getName() == "MainCamera") {
        camera->mainCamera = true;
    }
    camera->horizontalFOV = 0.785f; // cameraNode->mHorizontalFOV;
    camera->screenWidth = constants::SCREEN_WIDTH;
    camera->screenHeight = constants::SCREEN_HEIGHT;

    auto transform = mainCamera->getTransform();
    if (!transform) {
        transform = std::make_shared<Transform>(mainCamera);
        mainCamera->addComponent(transform);
    }
    transform->setPosition(cameraNode->mPosition.x, cameraNode->mPosition.y, cameraNode->mPosition.z);
    return mainCamera;
}

shared_ptr<Material> SceneLoader::loadMaterial(aiMesh* meshNode, const shared_ptr<GameObject>& parent) const
{
    const auto associatedMaterial = auxScene->mMaterials[meshNode->mMaterialIndex];
    auto material = std::make_shared<Material>(parent);

    float opacity;
    associatedMaterial->Get(AI_MATKEY_OPACITY, opacity);
    aiColor3D ambient;
    associatedMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    aiColor3D diffuse;
    associatedMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    aiColor3D specular;
    associatedMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    aiColor3D emission;
    associatedMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emission);
    float shininess;
    associatedMaterial->Get(AI_MATKEY_SHININESS, shininess);

    material->ambient = glm::vec4(ambient.r, ambient.g, ambient.b, opacity);
    material->diffuse = glm::vec4(diffuse.r, diffuse.g, diffuse.b, opacity);
    material->specular = glm::vec4(specular.r, specular.g, specular.b, 1.0f);
    material->emission = glm::vec4(emission.r, emission.g, emission.b, 0.0f);
    material->shininess = shininess;

    // Diffuse Map
    aiString texturePath;
    const auto numTextures = associatedMaterial->GetTextureCount(aiTextureType_DIFFUSE);
    if (numTextures > 0 && associatedMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
        material->diffuseMap = make_unique<Texture>(texturePath.C_Str());
    }

    parent->addComponent(material);
    return material;
}

aiLight* SceneLoader::getLightAssociated(aiNode* node) const
{
    auto i = 0u;
    while (i < auxScene->mNumLights) {
        const auto light = auxScene->mLights[i];
        if (light->mName == node->mName) {
            return light;
        }
        ++i;
    }
    return nullptr;
}

aiCamera* SceneLoader::getCameraAssociated(aiNode* node) const
{
    auto i = 0u;
    while (i < auxScene->mNumCameras) {
        const auto camera = auxScene->mCameras[i];
        if (camera->mName == node->mName) {
            return camera;
        }
        ++i;
    }
    return nullptr;
}
