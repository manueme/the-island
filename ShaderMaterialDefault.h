#pragma once
#include "Shader.h"

class Material;

class ShaderMaterialDefault : public Shader {
public:
    ShaderMaterialDefault(const shared_ptr<GameObject>& parent, const shared_ptr<Material>& material);
    ShaderType getShaderType() override;
    ~ShaderMaterialDefault();
    void setup(const shared_ptr<Material>& material, const shared_ptr<Mesh>& mesh) const;
    void setupLighting() const;
    void setupMaterial() const;
    void objectMounted() override;
    void setClippingPlane(glm::vec4 plane) const;
    shared_ptr<Material> material;
};
