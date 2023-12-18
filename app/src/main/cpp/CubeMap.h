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

    // Copy Constructor
    CubeMap(const CubeMap& other);

    CubeMap(const GLint& internalFormat, const GLint& param, const GLsizei& resolution, GLubyte* cubemapPixelBuffers[6]);

    // Assignment Operator
    CubeMap& operator=(const CubeMap& other);

    static CubeMap createSimpleTextureCubemap();

    int getResolution();

private:

    int resolution;

};


#endif //LIVEWALLPAPER05_CUBEMAP_H
