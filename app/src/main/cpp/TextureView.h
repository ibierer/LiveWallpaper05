//
// Created by Immanuel Bierer on 12/14/2023.
//

#ifndef LIVEWALLPAPER05_TEXTUREVIEW_H
#define LIVEWALLPAPER05_TEXTUREVIEW_H


#include "View.h"

class TextureView : public View {
public:

    GLuint mProgram;

    Texture texture;

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
            "uniform sampler2D environmentTexture;\n"
            "in vec3 position;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            //"    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
            "    outColor = texture(environmentTexture, position.xy); \n"
            "}\n";

    TextureView();

    ~TextureView();

    void render() override;

private:
};


#endif //LIVEWALLPAPER05_TEXTUREVIEW_H