#pragma once
#include "Component.h"
#include <vector>
#include <string>

class Texture;

class SkyBox : public Component {
public:
    SkyBox(const std::vector<std::string>& faces, const shared_ptr<GameObject>& parent);
    ComponentKey getComponentKey() override;
    ~SkyBox();
    void setupMesh();
    void lateRender() override;
private:
    shared_ptr<Texture> texture;
    unsigned int vao;
    unsigned int vertexBuffer;
};
