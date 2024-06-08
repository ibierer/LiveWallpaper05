//
// Created by Immanuel Bierer on 12/29/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H
#define LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H


#include "View.h"

class SphereWithReflectionView : public View {
public:

    GLuint sphereMapReflectionProgram;

    GLuint sphereMapBackgroundProgram;

    GLuint cubeMapReflectionProgram;

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

    const string SPHERE_MAP_REFLECTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            "void main() {\n"
            "    outColor = Texture(environmentTexture, reflect(normalize(direction), normalize(vNormal)));\n"
            "}\n";

    const string CUBE_MAP_REFLECTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, reflect(normalize(direction), normalize(vNormal)));\n"
            "}\n";

    SphereWithReflectionView();

    ~SphereWithReflectionView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H
