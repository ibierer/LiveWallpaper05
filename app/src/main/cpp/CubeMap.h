//
// Created by Immanuel Bierer on 12/11/2023.
//

#ifndef LIVEWALLPAPER05_CUBEMAP_H
#define LIVEWALLPAPER05_CUBEMAP_H


#include "EnvironmentMap.h"

class CubeMap : public EnvironmentMap {
public:

    // Default Constructor
    CubeMap();

    // Destructor
    ~CubeMap();

    CubeMap(const GLuint& textureId);

    // Copy Constructor
    CubeMap(const CubeMap& other);

    CubeMap(const GLint& internalFormat, const GLint& param, const GLsizei& resolution, GLubyte* cubemapPixelBuffers[6]);

    // Assignment Operator
    CubeMap& operator=(const CubeMap& other);

    static CubeMap createSimpleTextureCubemap();

private:

};


#endif //LIVEWALLPAPER05_CUBEMAP_H
