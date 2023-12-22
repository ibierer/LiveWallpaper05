//
// Created by Immanuel Bierer on 12/22/2023.
//

#ifndef LIVEWALLPAPER05_TEMPORARYCUBEMAPFBO_H
#define LIVEWALLPAPER05_TEMPORARYCUBEMAPFBO_H


class Texture;

#define DRAW_BUFFER 0

class TemporaryCubeMapFBO {
public:

    const GLenum drawBuffers[1] = {
            GL_COLOR_ATTACHMENT0
    };

    TemporaryCubeMapFBO();

    TemporaryCubeMapFBO(void* texture, const bool& addDepthBuffer, const bool& addStencilBuffer);

    // Copy constructor
    TemporaryCubeMapFBO(const TemporaryCubeMapFBO& other);

    // Assignment operator
    TemporaryCubeMapFBO& operator=(const TemporaryCubeMapFBO& other);

    ~TemporaryCubeMapFBO();

    int initialize(const bool& addDepthBuffer, const bool& addStencilBuffer);

    GLuint getWidth();

    GLuint getHeight();

    GLuint getFrameBuffer();

    GLuint getRenderedTextureId();

    template<class T> T& getRenderedTexture();

private:

    GLuint frameBuffer;

    void* renderedTexture;

    GLuint depthAndOrStencilRenderBuffer;

};


#endif //LIVEWALLPAPER05_TEMPORARYCUBEMAPFBO_H
