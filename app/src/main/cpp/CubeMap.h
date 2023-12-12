//
// Created by Immanuel Bierer on 12/11/2023.
//

#ifndef LIVEWALLPAPER05_CUBEMAP_H
#define LIVEWALLPAPER05_CUBEMAP_H


class CubeMap {
public:
    GLuint textureId;

    // Default Constructor
    CubeMap();

    CubeMap(const GLuint& textureId);

    // Copy Constructor
    CubeMap(const CubeMap& other);

    // Assignment Operator
    CubeMap& operator=(const CubeMap& other);

    static CubeMap genCubeMap(const GLint& internalFormat, const GLint& param, const GLsizei& resolution, GLubyte* cubemapPixelBuffers[6]);

    static CubeMap createSimpleTextureCubemap();
};


#endif //LIVEWALLPAPER05_CUBEMAP_H
