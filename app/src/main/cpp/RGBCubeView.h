//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_RGBCUBEVIEW_H
#define LIVEWALLPAPER05_RGBCUBEVIEW_H


#include "View.h"
#include "VertexArrayObject.h"

class RGBCubeView : public View {
public:

    VertexArrayObject mBoxVAO;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(COLOR_ATTRIBUTE_LOCATION) ") in vec3 color;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0f);\n"
            "    vColor = vec4(color, 1.0f);\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "in vec4 vColor;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vColor;\n"
            "}\n";

    RGBCubeView();

    ~RGBCubeView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_RGBCUBEVIEW_H
