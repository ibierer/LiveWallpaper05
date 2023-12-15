//
// Created by Immanuel Bierer on 12/14/2023.
//

#include "Texture.h"

// Function to map a value from one range to another
double Texture::map(double value, double in_min, double in_max, double out_min, double out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Function to generate Mandelbrot fractal and store RGB values in the output array
void Texture::generateMandelbrot(unsigned char* image) {
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

// Function to generate Mandelbrot fractal and store RGB values in the output array
void Texture::generateMSPaintColors(_vec3<GLubyte>* pixelBuffer) {
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){
            vec3 rgb;
            if(j < WIDTH / 6){
                rgb = vec3(
                        255.0f,
                        255.0f * 6.0f * j / WIDTH,
                        0.0f);
            }else if(j < 2 * WIDTH / 6){
                rgb = vec3(
                        (255.0f * WIDTH - 255.0f * (6 * j - WIDTH)) / WIDTH,
                        255.0f,
                        0.0f);
            }else if(j < 3 * WIDTH / 6){
                rgb = vec3(
                        0.0f,
                        255.0f,
                        255.0f * 6.0f * (j - WIDTH / 3) / WIDTH);
            }else if(j < 4 * WIDTH / 6){
                rgb = vec3(
                        0.0f,
                        255.0f - 255.0f * (6 * j - 3 * WIDTH) / WIDTH,
                        255.0f);
            }else if(j < 5 * WIDTH / 6){
                rgb = vec3(
                        255.0f * 6.0f * (j - 2 * WIDTH / 3) / WIDTH,
                        0.0f,
                        255.0f);
            }else{
                rgb = vec3(
                        255.0f,
                        0.0f,
                        255.0f - 255.0f * (6 * j - 5 * WIDTH) / WIDTH);
            }
            float ratio = (float)i / HEIGHT;
            float inverse = 1.0f - ratio;
            vec3 grey = vec3(127.0f);
            vec3 sum = ratio * rgb + inverse * grey;
            pixelBuffer[i * WIDTH + j] = _vec3<GLubyte>(sum.r, sum.b, sum.g);
        }
    }
}

Texture::Texture(){
    *this = Texture(MS_PAINT_COLORS);
}

Texture::Texture(ImageOption option){
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