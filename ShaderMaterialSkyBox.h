#pragma once
#include "Shader.h"

class ShaderMaterialSkyBox : public Shader {
public:
    ShaderMaterialSkyBox(const shared_ptr<GameObject>& parent);
    ShaderType getShaderType() override;
    ~ShaderMaterialSkyBox();
};
