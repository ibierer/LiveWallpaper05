//
// Created by Immanuel Bierer on 12/16/2023.
//

#ifndef LIVEWALLPAPER05_RENDERTOCUBEMAPVIEW_H
#define LIVEWALLPAPER05_RENDERTOCUBEMAPVIEW_H


class RenderToCubeMapView : public View {
public:

    GLuint _mProgram;

    CubeMap cubeMap;

    Texture texture;

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

    const string _VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "out vec3 position;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    position = pos;\n"
            "}\n";

    const string _FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 position;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, position.xy); \n"
            "}\n";

    RenderToCubeMapView();

    ~RenderToCubeMapView();

    void render() override;

    GLuint frameBuffers[6];
    GLuint depthAndStencilRenderBuffers[6];
    const GLenum drawBuffers[1] = {
            GL_COLOR_ATTACHMENT0
    };

    int initialize();

private:

};


#endif //LIVEWALLPAPER05_RENDERTOCUBEMAPVIEW_H
