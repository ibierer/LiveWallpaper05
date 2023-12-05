//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_BOX_H
#define LIVEWALLPAPER05_BOX_H


#include "Wallpaper.h"

class Box : public Wallpaper {
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

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

    Box();

    ~Box();

    void render() override;

private:
};


#endif //LIVEWALLPAPER05_BOX_H
