//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "Texture.h"

vec3 Texture::fetchFromSpectrum(const float &value) {
    float index = value - std::floor(value);
    if(index < 1.0f / 6.0f){
        return vec3(
                1.0f,
                6.0f * index,
                0.0f);
    }else if(index < 2.0f / 6.0f){
        return vec3(
                2.0f - 6.0f * index,
                1.0f,
                0.0f);
    }else if(index < 3.0f / 6.0f){
        return vec3(
                0.0f,
                1.0f,
                6.0f * index - 2.0f);
    }else if(index < 4.0f / 6.0f){
        return vec3(
                0.0f,
                4.0f - 6.0f * index,
                1.0f);
    }else if(index < 5.0f / 6.0f){
        return vec3(
                6.0f * index - 4.0f,
                0.0f,
                1.0f);
    }else{
        return vec3(
                1.0f,
                0.0f,
                6.0f * (1.0f - index));
    }
}

// Function to generate Mandelbrot fractal and store RGB values in the output array
void Texture::generateMandelbrot(unsigned char* image, const int& WIDTH, const int& HEIGHT) {

    // Maximum number of iterations for Mandelbrot algorithm
    const int MAX_ITERATIONS = 1536;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Map pixel coordinates to the complex plane
            double minX = -2.0;
            double maxX = 2.0;
            double minY = -1.5;
            double maxY = 1.5;
            double real = minX + x * (maxX - minX) / (WIDTH - 1);
            double imag = minY + y * (maxY - minY) / (HEIGHT - 1);

            std::complex<double> c(real, imag);
            std::complex<double> z = 0;

            int iterations = 0;

            // Iterate to check if the point is in the Mandelbrot set
            while (iterations < MAX_ITERATIONS && abs(z) < 2) {
                z = z * z + c;
                iterations++;
            }

            if(iterations == MAX_ITERATIONS){
                ((_vec3<unsigned char>*)image)[y * WIDTH + x] = _vec3<unsigned char>(0, 0, 0);
            }else{
                ((_vec3<unsigned char>*)image)[y * WIDTH + x] = 255.0f * fetchFromSpectrum(0.005f * iterations);
            }
        }
    }
}

// Function to generate Mandelbrot fractal and store RGB values in the output array
void Texture::generateMSPaintColors(_vec3<GLubyte>* pixelBuffer, const int& WIDTH, const int& HEIGHT) {
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){
            vec3 rgb = fetchFromSpectrum((float)j / WIDTH);
            float ratio = (float)i / HEIGHT;
            float inverse = 1.0f - ratio;
            vec3 grey = vec3(0.5f);
            vec3 sum = ratio * rgb + inverse * grey;
            pixelBuffer[i * WIDTH + j] = _vec3<GLubyte>(sum.r * 255.0f, sum.b * 255.0f, sum.g * 255.0f);
        }
    }
}

Texture::Texture(){

}

Texture::Texture(const DefaultImages& option, const int& w, const int& h){
    generateTexture(option, w, h);
}

Texture::Texture(const GLuint& texture, const int& w, const int& h){
    textureId = texture;
    width = w;
    height = h;
}

// Destructor
Texture::~Texture() {

}

// Copy Constructor
Texture::Texture(const Texture& other) : textureId(other.textureId), width(other.width), height(other.height) {

}

// Assignment Operator
Texture& Texture::operator=(const Texture& other) {
    if (this != &other) {
        textureId = other.textureId;
        width = other.width;
        height = other.height;
    }

    return *this;
}

GLuint Texture::getTextureId() {
    return textureId;
}

void Texture::generateTexture(const DefaultImages& option, const int& w, const int& h) {
    _vec3<GLubyte>* pixelBuffer = (_vec3<GLubyte>*)malloc(w * h * sizeof(_vec3<GLubyte>));
    switch(option){
        case MS_PAINT_COLORS:
            generateMSPaintColors(pixelBuffer, w, h);
            break;
    }
    *this = Texture(GL_RGB, w, h, (float*)pixelBuffer, GL_LINEAR);
    free(pixelBuffer);
}

Texture::Texture(const GLint& internalFormat, const int& width, const int& height, const float *pixelBuffer, const GLenum param) : width(width), height(height) {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer == NULL ? 0 : (const GLvoid*)pixelBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}
