//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMap.h"

// Default Constructor
SphereMap::SphereMap() {
    *this = SphereMap(Texture::MS_PAINT_COLORS);
}

SphereMap::SphereMap(Texture::ImageOption option){
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    const int resolution = 1536;
    _vec3<GLubyte>* pixelBuffer = (_vec3<GLubyte>*)malloc(resolution * resolution * sizeof(_vec3<GLubyte>));
    switch(option){
        case MS_PAINT_COLORS:
            generateMSPaintColors(pixelBuffer);
            break;
        case MANDELBROT:
            generateMandelbrot((unsigned char*)pixelBuffer);
            break;
    }
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            resolution,
            resolution,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            pixelBuffer);
    free(pixelBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// Destructor
SphereMap::~SphereMap() {

}

// Copy Constructor
SphereMap::SphereMap(const SphereMap& other) : textureId(other.textureId) {

}

// Assignment Operator
SphereMap& SphereMap::operator=(const SphereMap& other) {
    if (this != &other) {
        textureId = other.textureId;
    }

    return *this;
}

GLuint SphereMap::getTextureId() {
    return textureId;
}