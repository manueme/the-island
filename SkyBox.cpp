#include "SkyBox.h"
#include "OpenGLImports.h"
#include "Texture.h"
#include "ShaderMaterialSkyBox.h"
#include "Camera.h"
#include "GameObject.h"
#include <glm/gtc/matrix_transform.hpp>


SkyBox::SkyBox(const std::vector<std::string>& faces, const shared_ptr<GameObject>& parent)
    : Component("sky_box", parent)
{
    this->texture = make_shared<Texture>(faces);
    setupMesh();
}

ComponentKey SkyBox::getComponentKey()
{
    return SkyBoxComponent;
}


SkyBox::~SkyBox()
{
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vertexBuffer);
}

void SkyBox::setupMesh()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vertexBuffer);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
}

void SkyBox::lateRender()
{
    const auto currentCamera = parent->currentCamera;
    const auto shaderSkyBox = parent->getComponentFirst(ShaderComponent);
    if (shaderSkyBox) {
        const auto shader = static_pointer_cast<ShaderMaterialSkyBox>(shaderSkyBox);
        GLint oldCullFaceMode;
        glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
        GLint oldDepthFuncMode;
        glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

        glCullFace(GL_FRONT);
        glDepthFunc(GL_LEQUAL);

        shader->use();
        //******Camera Setup********
        const auto projection = glm::perspective(currentCamera->horizontalFOV, currentCamera->screenWidth / currentCamera->screenHeight, constants::NEAR_RENDER_PLANE, constants::FAR_RENDER_PLANE);
        const auto view = glm::mat4(glm::mat3(currentCamera->getViewMatrix()));
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        //**************************

        //******Texture Setup*******
        glBindVertexArray(vao);
        shader->setInt("skybox", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture->getData());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glCullFace(oldCullFaceMode);
        glDepthFunc(oldDepthFuncMode);
    }
}
