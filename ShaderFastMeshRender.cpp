#include "ShaderFastMeshRender.h"


ShaderFastMeshRender::ShaderFastMeshRender(const shared_ptr<GameObject>& parent)
    : Shader("FastMeshShader.vert", "FastMeshShader.frag", parent) {}

ShaderType ShaderFastMeshRender::getShaderType()
{
    return FastMeshRenderShader;
}

void ShaderFastMeshRender::setMatrixViewProjection(const glm::mat4 matrixViewProjection)
{
    setMat4("matrixViewProjection", matrixViewProjection);
    this->matrixViewProjection = matrixViewProjection;
}

glm::mat4 ShaderFastMeshRender::getCurrentMatrixViewProjection() const
{
    return matrixViewProjection;
}


ShaderFastMeshRender::~ShaderFastMeshRender() = default;
