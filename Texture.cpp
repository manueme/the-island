#include "Texture.h"
#include "OpenGLImports.h"
#include "FreeImage.h"

Texture::Texture(const string& path, const bool repeat)
{
    texture = 0;
    const auto fif = FreeImage_GetFIFFromFilename(path.c_str());
    auto bitmap = FreeImage_Load(fif, path.c_str());
    bitmap = FreeImage_ConvertTo24Bits(bitmap);
    const auto w = FreeImage_GetWidth(bitmap);
    const auto h = FreeImage_GetHeight(bitmap);
    void* data = FreeImage_GetBits(bitmap);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP);

    glBindTexture(GL_TEXTURE_2D, NULL);

    error = glGetError();

    FreeImage_Unload(bitmap);
}

Texture::Texture(vector<string> faces)
{
    error = glGetError();
    texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (unsigned int i = 0; i < faces.size(); i++) {
        const auto fif = FreeImage_GetFIFFromFilename(faces[i].c_str());
        auto bitmap = FreeImage_Load(fif, faces[i].c_str());
        bitmap = FreeImage_ConvertTo24Bits(bitmap);
        FreeImage_FlipVertical(bitmap);
        const auto w = FreeImage_GetWidth(bitmap);
        const auto h = FreeImage_GetHeight(bitmap);
        void* data = FreeImage_GetBits(bitmap);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0,
                         GL_RGB,
                         w,
                         h,
                         0,
                         GL_BGR_EXT,
                         GL_UNSIGNED_BYTE,
                         data
                );
            FreeImage_Unload(bitmap);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

unsigned int Texture::getData() const
{
    return texture;
}

Texture::~Texture()
{
    //Delete texture
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
}
