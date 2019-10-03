#pragma once
#include "GameObject.h"

class Water : public GameObject {
public:
    Water(const string& name, const shared_ptr<GameObject>& parent = nullptr);
    ~Water();

    void setupRefraction() const;
    void endRefraction();
    void setupReflection() const;
    void endReflection();

    int refractionMapOpenGlBind;
    int reflectionMapOpenGlBind;

    unsigned int getReflectionTexture() const;
    unsigned int getRefractionTexture() const;
    unsigned int getRefractionDepthTexture() const;

    int bindReflectionTexture() const;
    int bindRefractionTexture() const;

    void renderWater();

private:
    unsigned int refractionFrameBuffer;
    unsigned int refractionTexture;
    unsigned int refractionDepthTexture;

    unsigned int reflectionFrameBuffer;
    unsigned int reflectionTexture;
    unsigned int reflectionDepthBuffer;
};
