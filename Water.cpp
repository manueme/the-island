#include "Water.h"
#include "OpenGLImports.h"
#include "Mesh.h"

Water::Water(const string& name, const shared_ptr<GameObject>& parent)
    : GameObject(name, parent)
{
    // REFLECTION COLOR
    glGenFramebuffers(1, &reflectionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
    glGenTextures(1, &reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, static_cast<void*>(nullptr));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
    GLenum drawBuffers1[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers1); // "1" is the size of DrawBuffers
    glReadBuffer(GL_NONE);

    // REFLECTION DEPTH
    glGenRenderbuffers(1, &reflectionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // REFRACTION
    glGenFramebuffers(1, &refractionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, 0, GL_BGR, GL_UNSIGNED_BYTE, static_cast<void*>(nullptr));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
    GLenum drawBuffers2[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers2); // "1" is the size of DrawBuffers
    glReadBuffer(GL_NONE);

    glGenTextures(1, &refractionDepthTexture);
    glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT,
                 constants::SCREEN_WIDTH,
                 constants::SCREEN_HEIGHT,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


Water::~Water()
{
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteTextures(1, &refractionDepthTexture);
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);
}

void Water::setupRefraction() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClearDepth(1.0f);
}

void Water::endRefraction()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_FRAMEBUFFER, 0);
}

void Water::setupReflection() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClearDepth(1.0f);
}

void Water::endReflection()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Water::getReflectionTexture() const
{
    //get the resulting texture
    return reflectionTexture;
}

unsigned int Water::getRefractionTexture() const
{
    //get the resulting texture
    return refractionTexture;
}

unsigned int Water::getRefractionDepthTexture() const
{
    //get the resulting depth texture
    return refractionDepthTexture;
}

int Water::bindReflectionTexture() const
{
    glActiveTexture(GL_TEXTURE0 + constants::REFLECTION_MAP_GL_PLACE);
    glBindTexture(GL_TEXTURE_2D, getReflectionTexture());
    return constants::REFLECTION_MAP_GL_PLACE;
}

int Water::bindRefractionTexture() const
{
    glActiveTexture(GL_TEXTURE0 + constants::REFRACTION_MAP_GL_PLACE);
    glBindTexture(GL_TEXTURE_2D, getRefractionTexture());
    return constants::REFRACTION_MAP_GL_PLACE;
}

void Water::renderWater()
{
    auto waterMesh = static_pointer_cast<Mesh>(getComponentFirst(MeshComponent));
    if (waterMesh) {
        waterMesh->forceRenderMesh();
    }
}
