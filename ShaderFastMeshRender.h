#pragma once
#include "Shader.h"

class ShaderFastMeshRender : public Shader {
public:
    ShaderFastMeshRender(const shared_ptr<GameObject>& parent);
    ShaderType getShaderType() override;
    void setMatrixViewProjection(glm::mat4 matrixViewProjection);
    glm::mat4 getCurrentMatrixViewProjection() const;
    ~ShaderFastMeshRender();
private:
    glm::mat4 matrixViewProjection;
};
