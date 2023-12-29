//
// Created by Immanuel Bierer on 12/29/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H
#define LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H


#include "View.h"

class SphereWithReflectionView : public View {
public:

    VertexArrayObject sphereVAO;

    VertexArrayObject environmentTriangleVAO;

    CubeMap cubeMap;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "out vec3 vNormal;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    vNormal = normal;\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
            "}\n";

    const string REFLECTION_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string REFLECTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, direction); \n"
            "}\n";

    const string BACKGROUND_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string BACKGROUND_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, direction); \n"
            "}\n";

    SphereWithReflectionView();

    ~SphereWithReflectionView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H
