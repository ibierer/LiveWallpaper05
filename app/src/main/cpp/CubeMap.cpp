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
    GLubyte* cubemapPixelBuffers[6];
    for (int i = 0; i < 6; i++) {
        cubemapPixelBuffers[i] = (GLubyte*)malloc(3 * resolution * resolution * sizeof(unsigned char));
    }
    for (uint i = 0; i < resolution; i++) {
        for (uint j = 0; j < resolution; j++) {
            uint offset = 3 * (i * resolution + j);
            uint r = offset;
            uint g = offset + 1;
            uint b = offset + 2;
            cubemapPixelBuffers[0][r] = 255 - j;
            cubemapPixelBuffers[0][b] = 255;
            cubemapPixelBuffers[1][r] = j;
            cubemapPixelBuffers[1][g] = 255 - i;
            cubemapPixelBuffers[0][g] = 255 - i;
            cubemapPixelBuffers[1][b] = 0;
            cubemapPixelBuffers[2][r] = i;
            cubemapPixelBuffers[2][g] = 255;
            cubemapPixelBuffers[2][b] = j;
            cubemapPixelBuffers[3][r] = 255 - i;
            cubemapPixelBuffers[3][g] = 0;
            cubemapPixelBuffers[3][b] = j;
            cubemapPixelBuffers[4][r] = 255;
            cubemapPixelBuffers[4][g] = 255 - i;
            cubemapPixelBuffers[4][b] = j;
            cubemapPixelBuffers[5][r] = 0;
            cubemapPixelBuffers[5][g] = 255 - i;
            cubemapPixelBuffers[5][b] = 255 - j;
        }
    }
    CubeMap cubeMap = genCubeMap(GL_RGB, GL_LINEAR, resolution, cubemapPixelBuffers);
    for (int i = 0; i < 6; i++) {
        free(cubemapPixelBuffers[i]);
    }
    return cubeMap;
}

GLuint CubeMap::getTextureId() {
    return textureId;
}
