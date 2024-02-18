//
// Created by Immanuel Bierer on 12/16/2023.
//

#ifndef LIVEWALLPAPER05_FBO_H
#define LIVEWALLPAPER05_FBO_H


class Texture;

#define DRAW_BUFFER 0

class FBO {
public:

    const GLenum drawBuffers[1] = {
            GL_COLOR_ATTACHMENT0
    };

    FBO();

    FBO(void* texture, const bool& addDepthBuffer, const bool& addStencilBuffer);

    // Copy constructor
    FBO(const FBO& other);

    // Assignment operator
    FBO& operator=(const FBO& other);

    ~FBO();

    int initialize(const bool& addDepthBuffer, const bool& addStencilBuffer);

    GLuint getWidth();

    GLuint getHeight();

    GLuint getFrameBuffer();

    GLuint getRenderedTextureId();

    template<class T> T& getRenderedTexture();

private:

    GLuint frameBuffer;

    void* renderedTexture = nullptr;

    GLuint depthAndOrStencilRenderBuffer;

};


#endif //LIVEWALLPAPER05_FBO_H
