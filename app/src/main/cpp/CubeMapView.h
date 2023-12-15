//
// Created by Immanuel Bierer on 12/11/2023.
//

#ifndef LIVEWALLPAPER05_CUBEMAPVIEW_H
#define LIVEWALLPAPER05_CUBEMAPVIEW_H


#include "CubeMap.h"

class CubeMapView : public View {
public:

    GLuint mProgram;

    CubeMap cubeMap;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, direction); \n"
            "}\n";

    CubeMapView();

    ~CubeMapView();

    void render() override;

private:
};


#endif //LIVEWALLPAPER05_CUBEMAPVIEW_H
