#pragma once
#include "Component.h"
#include "Texture.h"
#include <string>
#include <glm/glm.hpp>

class Material : public Component {
public:
    Material(const shared_ptr<GameObject>& parent);
    void setWireframe(bool w);
    static string getLocalPath(const string& path);
    ~Material();
    ComponentKey getComponentKey() override;
    glm::vec4 diffuse;
    unique_ptr<Texture> diffuseMap;
    glm::vec4 specular;
    unique_ptr<Texture> specularMap;
    glm::vec4 ambient;
    glm::vec4 emission;

    float shininess, strength;
    bool wireframe = false;
private:

    string error = "";

};
