//
// Created by Immanuel Bierer on 12/11/2023.
//

#ifndef LIVEWALLPAPER05_CUBEMAPVIEW_H
#define LIVEWALLPAPER05_CUBEMAPVIEW_H


class CubeMapView : public Wallpaper {
public:

    GLuint mProgram;

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec3 position;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    position = pos;\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 position;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, position); \n"
            "}\n";

    CubeMapView();

    ~CubeMapView();

    void render() override;

private:
};


#endif //LIVEWALLPAPER05_CUBEMAPVIEW_H
