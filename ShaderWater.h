#pragma once
#include "Shader.h"

class Texture;

class Mesh;

class ShaderWater : public Shader {
public:
    ShaderWater(const shared_ptr<GameObject>& parent);
    ComponentKey getComponentKey() override;
    ShaderType getShaderType() override;
    void setup(Mesh* mesh) const;
    void addReflectionTexture(int texture) const;
    void addRefractionTexture(int texture) const;
    void update() override;
    ~ShaderWater();
private:
    unique_ptr<Texture> waterDistortionTexture;
    int moveFactor;
};
