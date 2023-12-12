//
// Created by Immanuel Bierer on 12/11/2023.
//

#include "CubeMap.h"

// Default Constructor
CubeMap::CubeMap() {

}

// Destructor
CubeMap::~CubeMap() {

}

CubeMap::CubeMap(const GLuint& textureId) : textureId(textureId) {

}

// Copy Constructor
CubeMap::CubeMap(const CubeMap& other) : textureId(other.textureId) {

}

// Assignment Operator
CubeMap& CubeMap::operator=(const CubeMap& other) {
    if (this != &other) {
        textureId = other.textureId;
    }

    return *this;
}

CubeMap CubeMap::genCubeMap(const GLint& internalFormat, const GLint& param, const GLsizei& resolution, GLubyte* cubemapPixelBuffers[6]) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (int i = 0; i < 6; i++) {
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                internalFormat,
                resolution,
                resolution,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                cubemapPixelBuffers == NULL ? 0 : cubemapPixelBuffers[i]);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);

    return CubeMap(texture);
}

CubeMap CubeMap::createSimpleTextureCubemap() {
    const GLsizei resolution = 256;

    _vec3<GLubyte>* cubemapPixelBuffers[6];

    for (int i = 0; i < 6; i++) {
        cubemapPixelBuffers[i] = (_vec3<GLubyte>*)malloc(resolution * resolution * sizeof(_vec3<GLubyte>));
    }

    for (uint i = 0; i < resolution; i++) {
        for (uint j = 0; j < resolution; j++) {
            uint texel = i * resolution + j;
            cubemapPixelBuffers[0][texel].r = 255 - j;
            cubemapPixelBuffers[0][texel].b = 255;
            cubemapPixelBuffers[0][texel].g = 255 - i;
            cubemapPixelBuffers[1][texel].r = j;
            cubemapPixelBuffers[1][texel].g = 255 - i;
            cubemapPixelBuffers[1][texel].b = 0;
            cubemapPixelBuffers[2][texel].r = i;
            cubemapPixelBuffers[2][texel].g = 255;
            cubemapPixelBuffers[2][texel].b = j;
            cubemapPixelBuffers[3][texel].r = 255 - i;
            cubemapPixelBuffers[3][texel].g = 0;
            cubemapPixelBuffers[3][texel].b = j;
            cubemapPixelBuffers[4][texel].r = 255;
            cubemapPixelBuffers[4][texel].g = 255 - i;
            cubemapPixelBuffers[4][texel].b = j;
            cubemapPixelBuffers[5][texel].r = 0;
            cubemapPixelBuffers[5][texel].g = 255 - i;
            cubemapPixelBuffers[5][texel].b = 255 - j;
        }
    }

    CubeMap cubeMap = genCubeMap(GL_RGB, GL_LINEAR, resolution, (GLubyte**)cubemapPixelBuffers);

    for (int i = 0; i < 6; i++) {
        free(cubemapPixelBuffers[i]);
    }

    return cubeMap;
}

GLuint CubeMap::getTextureId() {
    return textureId;
}
