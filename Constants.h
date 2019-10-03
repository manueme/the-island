#pragma once

enum ShaderType {
    MaterialDefaultShader,
    SkyBoxShader,
    FastMeshRenderShader,
    WaterShader
};

enum ComponentKey {
    MeshComponent = 0,
    LightComponent,
    MaterialComponent,
    ShaderComponent,
    CameraComponent,
    TransformComponent,
    SkyBoxComponent,
    ObjectAnimationComponent
};

namespace constants {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    const int SCREEN_FPS = 60;

    const unsigned int SHADOW_MAPS_WIDTH = 2048, SHADOW_MAPS_HEIGHT = 2048;
    const unsigned int WATER_MAPS_WIDTH = 1024, WATER_MAPS_HEIGHT = 1024;
    static const float NEAR_RENDER_PLANE = 0.1f;
    static const float FAR_RENDER_PLANE = 10000.0f;
    static const int SHADOW_MAP_GL_PLACE = 4;
    static const int REFLECTION_MAP_GL_PLACE = 5;
    static const int REFRACTION_MAP_GL_PLACE = 6;
    static const int WATER_DISTORTION_MAP_GL_PLACE = 7;
    static const int WATER_NORMAL_MAP_GL_PLACE = 8;
    static const int GENERIC_MATERIAL_GL_PLACE = 8;
} // namespace constants
