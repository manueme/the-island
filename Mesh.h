#pragma once
#include "OpenGLImports.h"
#include "Component.h"
#include <glm/glm.hpp>
#include <assimp/mesh.h>
#include <list>

class Material;
class Shader;

class Mesh : public Component {
public:
    Mesh(aiMesh* meshNode, const shared_ptr<GameObject>& parent);
    ComponentKey getComponentKey() override;
    void shadowMappingRender(const shared_ptr<ShaderFastMeshRender>& depthShader) override;
    void render() override;
    void lateRender() override;
    void forceRenderMesh();
    void update() override;
    void objectMounted() override;
    ~Mesh();

    glm::vec3 minPoints;
    glm::vec3 maxPoints;

    shared_ptr<Material> material;

    bool renderInLateRender = false;
    bool doNotRender = false;

    bool insideFrustum(const glm::mat4& projectionViewMatrix, const glm::mat4& modelTransformation) const;
    inline static bool checkFrustumCalc(glm::vec4 point);

private:

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    void loadMesh(aiMesh* meshNode);

    string error = "";

    int vertexCount;
    unsigned int faceCount;
    int indexSize;

    GLuint vao;
    GLuint indexBuffer;
    GLuint vertexBuffer;

    list<shared_ptr<Shader>> shaderList;
};
