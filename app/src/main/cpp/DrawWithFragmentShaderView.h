//
// Created by Immanuel Bierer on 12/17/2023.
//

#ifndef LIVEWALLPAPER05_DRAWWITHFRAGMENTSHADERVIEW_H
#define LIVEWALLPAPER05_DRAWWITHFRAGMENTSHADERVIEW_H


#include "View.h"
#include "Texture.h"
#include "FBO.h"

class DrawWithFragmentShaderView : public View {
public:

    GLuint mProgram;

    VertexArrayObject tilesVAO;

    Texture texture;

    FBO fbo;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "out vec3 position;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    position = pos;\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D image;\n"
            "in vec3 position;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(image, position.xy); \n"
            "}\n";

    DrawWithFragmentShaderView();

    ~DrawWithFragmentShaderView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_DRAWWITHFRAGMENTSHADERVIEW_H
