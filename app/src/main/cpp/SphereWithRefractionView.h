
//
// Created by Immanuel Bierer on 12/29/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREWITHREFRACTIONVIEW_H
#define LIVEWALLPAPER05_SPHEREWITHREFRACTIONVIEW_H


#include "View.h"

class SphereWithRefractionView : public View {
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
            "void main() {\n"
            "    outColor = Texture(environmentTexture, refract(normalize(direction), normalize(vNormal), 0.75));\n"
            "}\n";

    const string SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            DOUBLE_REFRACT_FUNCTION +
            "void main() {\n"
            "    outColor = Texture(environmentTexture, doubleRefract(normalize(direction), normalize(vNormal), 0.75));\n"
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
            "precision mediump float;\n"
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
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, refract(normalize(direction), normalize(vNormal), 0.75));\n"
            "}\n";

    const string CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            DOUBLE_REFRACT_FUNCTION +
            "void main() {\n"
            "    outColor = texture(environmentTexture, refract(normalize(direction), normalize(vNormal), 0.75));\n"
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

    SphereWithRefractionView();

    ~SphereWithRefractionView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREWITHREFRACTIONVIEW_H
