//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMap.h"

// mix function for floats
float mix(float x, float y, float a) {
    return x * (1.0 - a) + y * a;
}

// mix function for vectors
vec3 mix(vec3 x, vec3 y, float a) {
    return x * (1.0 - a) + y * a;
}

// Default Constructor
SphereMap::SphereMap() {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    const int resolution = 2048;
    _vec3<GLubyte>* pixelBuffer = (_vec3<GLubyte>*)malloc(resolution * resolution * sizeof(_vec3<GLubyte>));
    for(int i = 0; i < resolution; i++){
        for(int j = 0; j < resolution; j++){
            float r;
            float g;
            float b;
            if(j < resolution / 6){
                r = 255.0f;
                g = 255.0f * 6.0f * j / resolution;
                b = 0.0f;
            }else if(j < 2 * resolution / 6){
                r = (255.0f * resolution - 255.0f * (6 * j - resolution)) / resolution;
                g = 255.0f;
                b = 0.0f;
            }else if(j < 3 * resolution / 6){
                r = 0.0f;
                g = 255.0f;
                b = 255.0f * 6.0f * (j - resolution / 3) / resolution;
            }else if(j < 4 * resolution / 6){
                r = 0.0f;
                g = (255.0f - 255.0f * (6 * j - 3 * resolution) / resolution);
                b = 255.0f;
            }else if(j < 5 * resolution / 6){
                r = 255.0f * 6.0f * (j - 2 * resolution / 3) / resolution;
                g = 0.0f;
                b = 255.0f;
            }else{
                r = 255.0f;
                g = 0.0f;
                b = (255.0f - 255.0f * (6 * j - 5 * resolution) / resolution);
            }
            float ratio = (float)i / resolution;
            float inverse = 1.0f - ratio;
            r = mix(r, 127.0f, inverse);
            g = mix(g, 127.0f, inverse);
            b = mix(b, 127.0f, inverse);
            pixelBuffer[i * resolution + j] = _vec3<GLubyte>(floor(r), floor(g), floor(b));
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
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
