//
// Created by Immanuel Bierer on 12/28/2023.
//

#ifndef LIVEWALLPAPER05_CUBEVIEW_H
#define LIVEWALLPAPER05_CUBEVIEW_H


#include "View.h"

class CubeView : public View {
public:

    //VertexArrayObject cubeVAO;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
            "}\n";

    CubeView();

    ~CubeView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_CUBEVIEW_H
