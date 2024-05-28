//
// Created by Immanuel Bierer on 12/31/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREWITHFRESNELEFFECTVIEW_H
#define LIVEWALLPAPER05_SPHEREWITHFRESNELEFFECTVIEW_H


#include "View.h"

class SphereWithFresnelEffectView : public View {
public:

    GLuint sphereMapRefractionProgram;

    GLuint cubeMapDoubleRefractionProgram;

    GLuint sphereMapBackgroundProgram;

    GLuint sphereMapDoubleRefractionProgram;

    GLuint cubeMapRefractionProgram;

    GLuint cubeMapBackgroundProgram;

    VertexArrayObject sphereVAO;

    VertexArrayObject environmentTriangleVAO;

    EnvironmentMap environmentMap;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "uniform mat4 mvp;\n"
            "uniform mat4 viewTransformation;\n"
            "out vec3 direction;\n"
            "out vec3 vNormal;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    direction = (viewTransformation * vec4(pos, 1.0)).xyz;\n"
            "    vNormal = normal;\n"
            "}\n";

    const string SPHERE_MAP_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec4 reflectedColor = Texture(environmentTexture, reflect2(normalizedDirection, normalizedNormal, dotNI));\n"
            "    vec4 refractedColor = Texture(environmentTexture, refract2(normalizedDirection, normalizedNormal, 0.75, dotNI));\n"
            "    outColor = mix(refractedColor, reflectedColor, fresnel(dotNI));\n"
            "}\n";

    const string SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision highp float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            REFLECT2_FUNCTION +
            DOUBLE_REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec4 reflectedColor = Texture(environmentTexture, reflect2(normalizedDirection, normalizedNormal, dotNI));\n"
            "    vec4 refractedColor = Texture(environmentTexture, doubleRefract2(normalizedDirection, normalizedNormal, 0.75, dotNI));\n"
            "    outColor = mix(refractedColor, reflectedColor, fresnel(dotNI));\n"
            "}\n";

    const string BACKGROUND_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string SPHERE_MAP_BACKGROUND_FRAGMENT_SHADER =
            ES_VERSION +
            "precision highp float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            "void main() {\n"
            "    outColor = Texture(environmentTexture, direction);\n"
            "}\n";

    const string CUBE_MAP_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            REFLECT2_FUNCTION +
            REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec4 reflectedColor = texture(environmentTexture, reflect2(normalizedDirection, normalizedNormal, dotNI));\n"
            "    vec4 refractedColor = texture(environmentTexture, refract2(normalizedDirection, normalizedNormal, 0.75, dotNI));\n"
            "    outColor = mix(refractedColor, reflectedColor, fresnel(dotNI));\n"
            "}\n";

    const string CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            REFLECT2_FUNCTION +
            DOUBLE_REFRACT2_FUNCTION +
            FRESNEL_EFFECT_FUNCTION +
            "void main() {\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    vec3 normalizedNormal = normalize(vNormal);\n"
            "    float dotNI = dot(normalizedDirection, normalizedNormal);\n"
            "    vec4 reflectedColor = texture(environmentTexture, reflect2(normalizedDirection, normalizedNormal, dotNI));\n"
            "    vec4 refractedColor = texture(environmentTexture, doubleRefract2(normalizedDirection, normalizedNormal, 0.75, dotNI));\n"
            "    outColor = mix(refractedColor, reflectedColor, fresnel(dotNI));\n"
            "}\n";

    const string CUBE_MAP_BACKGROUND_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, direction); \n"
            "}\n";

    SphereWithFresnelEffectView(Texture::DefaultImages option,
                                const int &resolution);

    ~SphereWithFresnelEffectView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREWITHFRESNELEFFECTVIEW_H
