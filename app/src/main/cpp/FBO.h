//
// Created by Immanuel Bierer on 12/16/2023.
//

#ifndef LIVEWALLPAPER05_FBO_H
#define LIVEWALLPAPER05_FBO_H


#define DRAW_BUFFER 0

class FBO {
public:

    const GLenum drawBuffers[1] = {
            GL_COLOR_ATTACHMENT0
    };

    FBO();

    FBO(const int& width, const int& height, const bool& includeDepthBuffer, const bool& includeStencilBuffer);

    // Copy constructor
    FBO(const FBO& other);

    // Assignment operator
    FBO& operator=(const FBO& other);

    ~FBO();

    int initialize(const int& width, const int& height, const bool& includeDepthBuffer, const bool& includeStencilBuffer);

    GLuint getWidth();

    GLuint getHeight();

    GLuint getFrameBuffer();

    GLuint getRenderedTexture();

private:

    GLsizei width;

    GLsizei height;

    GLuint frameBuffer;

    GLuint renderedTexture;

    GLuint depthAndOrStencilRenderBuffer;

};


#endif //LIVEWALLPAPER05_FBO_H
