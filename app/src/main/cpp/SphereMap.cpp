//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMap.h"
#include <complex>

// Default Constructor
SphereMap::SphereMap() {
    *this = SphereMap(Texture::MS_PAINT_COLORS);
}

SphereMap::SphereMap(Texture::imageOption option) {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    const int resolution = 1536;
    _vec3<GLubyte>* pixelBuffer = (_vec3<GLubyte>*)malloc(resolution * resolution * sizeof(_vec3<GLubyte>));
    switch(option){
        case MS_PAINT_COLORS:
            generateMSPaintColors(pixelBuffer);
            break;
        case MANDLEBROT:
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

void SphereMap::generateMandelbrot(unsigned char *image) {
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            // Map pixel coordinates to the complex plane
            double real = map(x, 0, WIDTH, -2.0, 2.0);
            double imag = map(y, 0, HEIGHT, -1.5, 1.5);

            std::complex<double> c(real, imag);
            std::complex<double> z(0, 0);

            int iteration = 0;

            // Iterate until the escape condition is met or maximum iterations are reached
            while (std::abs(z) < 2.0 && iteration < MAX_ITERATIONS) {
                z = z * z + c;
                ++iteration;
            }

            // Map the iteration count to a color
            int color = map(iteration, 0, MAX_ITERATIONS, 0, COLOR_DEPTH);

            // Store RGB values in the image array
            int index = 3 * (y * WIDTH + x);
            image[index] = color * 100;            // Red channel
            image[index + 1] = color * 100 % 256;  // Green channel
            image[index + 2] = color * 100 % 256;  // Blue channel
        }
    }
}
