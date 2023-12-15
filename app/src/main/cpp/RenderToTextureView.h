//
// Created by Immanuel Bierer on 12/14/2023.
//

#ifndef LIVEWALLPAPER05_RENDERTOTEXTUREVIEW_H
#define LIVEWALLPAPER05_RENDERTOTEXTUREVIEW_H


#include "View.h"
#include "Texture.h"

#define DRAW_BUFFER 0

class FBO {
public:
    GLuint frameBuffer;
    GLuint depthAndStencilRenderBuffer;
    GLuint renderedTexture;
    GLsizei width;
    GLsizei height;
    const GLenum drawBuffers[1] = {
            GL_COLOR_ATTACHMENT0
    };

    FBO() {

    }

    ~FBO() {
        glDeleteFramebuffers(1, &frameBuffer);
        glDeleteRenderbuffers(1, &depthAndStencilRenderBuffer);
    }

    int initialize(int width, int height) {
        this->width = width;
        this->height = height;
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

        glGenTextures(1, &renderedTexture);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[DRAW_BUFFER], GL_TEXTURE_2D,
                               renderedTexture, 0);

        glGenRenderbuffers(1, &depthAndStencilRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthAndStencilRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  depthAndStencilRenderBuffer);

        bool frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        return frameBufferStatus != GL_FRAMEBUFFER_COMPLETE;
    }
};

class RenderToTextureView : public View {
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
            "    outColor = texture(environmentTexture, position.xy); \n"
            "}\n";

    RenderToTextureView();

    ~RenderToTextureView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_RENDERTOTEXTUREVIEW_H
