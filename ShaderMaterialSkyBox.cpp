#include "ShaderMaterialSkyBox.h"


ShaderMaterialSkyBox::ShaderMaterialSkyBox(const shared_ptr<GameObject>& parent)
    : Shader("SkyBoxShader.vert", "SkyBoxShader.frag", parent) {}

ShaderType ShaderMaterialSkyBox::getShaderType()
{
    return SkyBoxShader;
}


ShaderMaterialSkyBox::~ShaderMaterialSkyBox() = default;
