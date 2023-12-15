//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "Texture.h"

Texture::Texture(){
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    const int resolution = 1536;
    _vec3<GLubyte>* pixelBuffer = (_vec3<GLubyte>*)malloc(resolution * resolution * sizeof(_vec3<GLubyte>));
    for(int i = 0; i < resolution; i++){
        for(int j = 0; j < resolution; j++){
            vec3 rgb;
            if(j < resolution / 6){
                rgb = vec3(
                        255.0f,
                        255.0f * 6.0f * j / resolution,
                        0.0f);
            }else if(j < 2 * resolution / 6){
                rgb = vec3(
                        (255.0f * resolution - 255.0f * (6 * j - resolution)) / resolution,
                        255.0f,
                        0.0f);
            }else if(j < 3 * resolution / 6){
                rgb = vec3(
                        0.0f,
                        255.0f,
                        255.0f * 6.0f * (j - resolution / 3) / resolution);
            }else if(j < 4 * resolution / 6){
                rgb = vec3(
                        0.0f,
                        255.0f - 255.0f * (6 * j - 3 * resolution) / resolution,
                        255.0f);
            }else if(j < 5 * resolution / 6){
                rgb = vec3(
                        255.0f * 6.0f * (j - 2 * resolution / 3) / resolution,
                        0.0f,
                        255.0f);
            }else{
                rgb = vec3(
                        255.0f,
                        0.0f,
                        255.0f - 255.0f * (6 * j - 5 * resolution) / resolution);
            }
            float ratio = (float)i / resolution;
            float inverse = 1.0f - ratio;
            vec3 grey = vec3(127.0f);
            vec3 sum = ratio * rgb + inverse * grey;
            pixelBuffer[i * resolution + j] = _vec3<GLubyte>(sum.r, sum.b, sum.g);
        }
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
Texture::~Texture() {

}

// Copy Constructor
Texture::Texture(const Texture& other) : textureId(other.textureId) {

}

// Assignment Operator
Texture& Texture::operator=(const Texture& other) {
    if (this != &other) {
        textureId = other.textureId;
    }

    return *this;
}

GLuint Texture::getTextureId() {
    return textureId;
}
