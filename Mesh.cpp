#include "Mesh.h"
#include "Transform.h"
#include "GameObject.h"
#include "ShaderMaterialDefault.h"
#include "ShaderFastMeshRender.h"
#include "ShaderWater.h"
#include "Material.h"
#include "Camera.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


Mesh::Mesh(aiMesh* meshNode, const shared_ptr<GameObject>& parent)
    : Component("mesh", parent)
{
    loadMesh(meshNode);
}

void Mesh::objectMounted()
{
    const auto shaderCompList = parent->getComponentList(ShaderComponent);
    for (const auto& shader : shaderCompList) {
        shaderList.push_back(static_pointer_cast<Shader>(shader));
    }
}

ComponentKey Mesh::getComponentKey()
{
    return MeshComponent;
}

void Mesh::shadowMappingRender(const shared_ptr<ShaderFastMeshRender>& depthShader)
{
    if (!doNotRender) {
        const auto transform = parent->getTransform();
        glm::mat4 model;
        model = translate(model, transform->getPosition());
        model *= mat4_cast(transform->getRotation());
        model = scale(model, transform->getScale());
        if (insideFrustum(depthShader->getCurrentMatrixViewProjection(), model)) {
            depthShader->setMat4("model", model);
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }
}

void Mesh::render()
{
    const auto currentCamera = parent->currentCamera;
    const auto projection = glm::perspective(currentCamera->horizontalFOV, currentCamera->screenWidth / currentCamera->screenHeight, constants::NEAR_RENDER_PLANE, constants::FAR_RENDER_PLANE);
    const auto view = currentCamera->getViewMatrix();
    const auto transform = parent->getTransform();

    glm::mat4 model;
    model = translate(model, transform->getPosition());
    model *= mat4_cast(transform->getRotation());
    model = scale(model, transform->getScale());

    this->enabled = insideFrustum(projection * view, model);
    if (!doNotRender && this->enabled && !renderInLateRender) {
        forceRenderMesh();
    }
}

void Mesh::lateRender()
{
    if (!doNotRender && this->enabled && renderInLateRender) {
        forceRenderMesh();
    }
}

void Mesh::forceRenderMesh()
{
    for (const auto& shader : shaderList) {

        if (shader->enabled) {
            switch (shader->getShaderType()) {
            case MaterialDefaultShader: {
                static_pointer_cast<ShaderMaterialDefault>(shader)->setup(material, static_pointer_cast<Mesh>(shared_from_this()));
            }
            break;
            case WaterShader: {
                static_pointer_cast<ShaderWater>(shader)->setup(this);
            }
            break;
            default: ;
            }
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }
    }
}

void Mesh::update() {}


Mesh::~Mesh()
{
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &vertexBuffer);
}

bool Mesh::insideFrustum(const glm::mat4& projectionViewMatrix, const glm::mat4& modelTransformation) const
{
    // AABB calculation

    glm::vec4 points[8];
    // bottomLeftBack
    points[0] = (modelTransformation * glm::vec4(minPoints.x, minPoints.y, minPoints.z, 1.0));
    auto auxMinPoints = glm::vec3(points[0].x, points[0].y, points[0].z);
    auto auxMaxPoints = glm::vec3(points[0].x, points[0].y, points[0].z);
    // bottomRightBack
    points[1] = (modelTransformation * glm::vec4(maxPoints.x, minPoints.y, minPoints.z, 1.0));
    // bottomLeftFront
    points[2] = (modelTransformation * glm::vec4(minPoints.x, minPoints.y, maxPoints.z, 1.0));
    // bottomRightFront
    points[3] = (modelTransformation * glm::vec4(maxPoints.x, minPoints.y, maxPoints.z, 1.0));
    // topLeftBack
    points[4] = (modelTransformation * glm::vec4(minPoints.x, maxPoints.y, minPoints.z, 1.0));
    // topRightBack
    points[5] = (modelTransformation * glm::vec4(maxPoints.x, maxPoints.y, minPoints.z, 1.0));
    // topLeftFront
    points[6] = (modelTransformation * glm::vec4(minPoints.x, maxPoints.y, maxPoints.z, 1.0));
    // topRightFront
    points[7] = (modelTransformation * glm::vec4(maxPoints.x, maxPoints.y, maxPoints.z, 1.0));

    for (auto point : points) {
        if (auxMinPoints.x > point.x) {
            auxMinPoints.x = point.x;
        }
        if (auxMinPoints.y > point.y) {
            auxMinPoints.y = point.y;
        }
        if (auxMinPoints.z > point.z) {
            auxMinPoints.z = point.z;
        }
        if (auxMaxPoints.x < point.x) {
            auxMaxPoints.x = point.x;
        }
        if (auxMaxPoints.y < point.y) {
            auxMaxPoints.y = point.y;
        }
        if (auxMaxPoints.z < point.z) {
            auxMaxPoints.z = point.z;
        }
    }
    points[0] = projectionViewMatrix * glm::vec4(auxMinPoints.x, auxMinPoints.y, auxMinPoints.z, 1.0);
    // bottomRightBack
    points[1] = projectionViewMatrix * glm::vec4(auxMaxPoints.x, auxMinPoints.y, auxMinPoints.z, 1.0);
    // bottomLeftFront
    points[2] = projectionViewMatrix * glm::vec4(auxMinPoints.x, auxMinPoints.y, auxMaxPoints.z, 1.0);
    // bottomRightFront
    points[3] = projectionViewMatrix * glm::vec4(auxMaxPoints.x, auxMinPoints.y, auxMaxPoints.z, 1.0);
    // topLeftBack
    points[4] = projectionViewMatrix * glm::vec4(auxMinPoints.x, auxMaxPoints.y, auxMinPoints.z, 1.0);
    // topRightBack
    points[5] = projectionViewMatrix * glm::vec4(auxMaxPoints.x, auxMaxPoints.y, auxMinPoints.z, 1.0);
    // topLeftFront
    points[6] = projectionViewMatrix * glm::vec4(auxMinPoints.x, auxMaxPoints.y, auxMaxPoints.z, 1.0);
    // topRightFront
    points[7] = projectionViewMatrix * glm::vec4(auxMaxPoints.x, auxMaxPoints.y, auxMaxPoints.z, 1.0);
    // End AABB

    //  Frustum check for AABB

    auto outsideRight = true;
    auto outsideLeft = true;

    auto outsideTop = true;
    auto outsideBottom = true;

    auto outsideBack = true;
    auto outsideFront = true;

    for (auto clip : points) {

        outsideRight = outsideRight && (clip.x >= clip.w);
        outsideLeft = outsideLeft && (clip.x <= -clip.w);

        outsideTop = outsideTop && (clip.y >= clip.w);
        outsideBottom = outsideBottom && (clip.y <= -clip.w);

        outsideBack = outsideBack && (clip.z >= clip.w);
        outsideFront = outsideFront && (clip.z <= -clip.w);

    }
    return !(outsideRight || outsideLeft || outsideTop || outsideBottom || outsideBack || outsideFront);
}

inline bool Mesh::checkFrustumCalc(glm::vec4 point)
{
    return (abs(point.x) < point.w &&
            abs(point.y) < point.w &&
            0 < point.z &&
            point.z < point.w);
}

void Mesh::loadMesh(aiMesh* meshNode)
{
    vertexCount = meshNode->mNumVertices;
    std::vector<Vertex> vertices;
    for (auto i = 0; i < vertexCount; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = meshNode->mVertices[i].x;
        vector.y = meshNode->mVertices[i].y;
        vector.z = meshNode->mVertices[i].z;
        if (i == 0 || vector.x < minPoints.x) {
            minPoints.x = vector.x;
        }
        if (i == 0 || vector.y < minPoints.y) {
            minPoints.y = vector.y;
        }
        if (i == 0 || vector.z < minPoints.z) {
            minPoints.z = vector.z;
        }
        if (i == 0 || vector.x > maxPoints.x) {
            maxPoints.x = vector.x;
        }
        if (i == 0 || vector.y > maxPoints.y) {
            maxPoints.y = vector.y;
        }
        if (i == 0 || vector.z > maxPoints.z) {
            maxPoints.z = vector.z;
        }
        vertex.position = vector;

        vector.x = meshNode->mNormals[i].x;
        vector.y = meshNode->mNormals[i].y;
        vector.z = meshNode->mNormals[i].z;

        vertex.normal = vector;
        if (meshNode->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            float intPart;
            vec.x = meshNode->mTextureCoords[0][i].x;
            vec.x = modff(vec.x, &intPart);
            vec.y = meshNode->mTextureCoords[0][i].y;
            vec.y = modff(vec.y, &intPart);
            vertex.texCoords = vec;
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // ****************** INDICES ***************

    faceCount = meshNode->mNumFaces;
    vector<unsigned int> indices;
    for (unsigned int i = 0; i < faceCount; i++) {
        auto face = meshNode->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);

        }

    }
    indexSize = indices.size();

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexSize, &indices[0], GL_STATIC_DRAW);

    indices.clear();

    // *************** VERTEX BUFFER ***************

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    vertices.clear();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(nullptr));
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoords)));
    // vertex index
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
