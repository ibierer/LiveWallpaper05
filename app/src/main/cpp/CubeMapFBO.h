//
// Created by Immanuel Bierer on 12/17/2023.
//

#ifndef LIVEWALLPAPER05_CUBEMAPFBO_H
#define LIVEWALLPAPER05_CUBEMAPFBO_H


class CubeMapFBO {
public:

    GLuint frameBuffers[6];

    GLuint depthAndOrStencilRenderBuffers[6];

    CubeMap cubeMap;

    const GLenum drawBuffers[1] = {
            GL_COLOR_ATTACHMENT0
    };

    // Default constructor
    CubeMapFBO();

    CubeMapFBO(CubeMap cubeMap, const bool& addDepthBuffer, const bool& addStencilBuffer);

    // Copy constructor
    CubeMapFBO(const CubeMapFBO& other);

    // Assignment constructor
    CubeMapFBO& operator=(const CubeMapFBO& other);

    // Destructor
    ~CubeMapFBO();

    int getResolution();

    GLuint getTextureId();

private:

};


#endif //LIVEWALLPAPER05_CUBEMAPFBO_H
