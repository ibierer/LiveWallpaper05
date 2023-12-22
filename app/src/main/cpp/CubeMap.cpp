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

// Copy Constructor
CubeMap::CubeMap(const CubeMap& other) {
    this->textureId = other.textureId;
    this->resolution = other.resolution;
    width = other.width;
    height = other.height;
}

// Assignment Operator
CubeMap& CubeMap::operator=(const CubeMap& other) {
    if (this != &other) {
        textureId = other.textureId;
        resolution = other.resolution;
        width = other.width;
        height = other.height;
    }

    return *this;
}

CubeMap CubeMap::createSimpleTextureCubemap() {
    const GLsizei resolution = 256;

    _vec3<GLubyte>* cubeMapPixelBuffers[6];

    for (int i = 0; i < 6; i++) {
        cubeMapPixelBuffers[i] = (_vec3<GLubyte>*)malloc(resolution * resolution * sizeof(_vec3<GLubyte>));
    }

    for (uint i = 0; i < resolution; i++) {
        for (uint j = 0; j < resolution; j++) {
            uint texel = i * resolution + j;
            cubeMapPixelBuffers[0][texel] = _vec3<GLubyte>(255 - j, 255, 255 - i);
            cubeMapPixelBuffers[1][texel] = _vec3<GLubyte>(j, 255 - i, 0);
            cubeMapPixelBuffers[2][texel] = _vec3<GLubyte>(i, 255, j);
            cubeMapPixelBuffers[3][texel] = _vec3<GLubyte>(255 - i, 0, j);
            cubeMapPixelBuffers[4][texel] = _vec3<GLubyte>(255, 255 - i, j);
            cubeMapPixelBuffers[5][texel] = _vec3<GLubyte>(0, 255 - i, 255 - j);
        }
    }

    CubeMap cubeMap = CubeMap(GL_RGB, GL_LINEAR, resolution, (GLubyte**)cubeMapPixelBuffers);

    for (int i = 0; i < 6; i++) {
        free(cubeMapPixelBuffers[i]);
    }

    return cubeMap;
}

CubeMap::CubeMap(const GLint &internalFormat, const GLint &param, const GLsizei &resolution, GLubyte **cubemapPixelBuffers) {
    this->resolution = resolution;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    width = resolution;
    height = resolution;

    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, resolution, resolution, 0, GL_RGB, GL_UNSIGNED_BYTE, cubemapPixelBuffers == NULL ? 0 : cubemapPixelBuffers[i]);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

int CubeMap::getResolution() {
    return resolution;
}
