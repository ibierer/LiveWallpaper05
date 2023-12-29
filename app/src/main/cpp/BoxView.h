//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_BOXVIEW_H
#define LIVEWALLPAPER05_BOXVIEW_H


#include "View.h"
#include "VertexArrayObject.h"

class BoxView : public View {
public:

    VertexArrayObject mBoxVAO;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    BoxView();

    ~BoxView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_BOXVIEW_H
