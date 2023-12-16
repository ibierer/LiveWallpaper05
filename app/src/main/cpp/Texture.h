//
// Created by Immanuel Bierer on 12/14/2023.
//

#ifndef LIVEWALLPAPER05_TEXTURE_H
#define LIVEWALLPAPER05_TEXTURE_H


#include <complex>

class Texture {
public:

    enum ImageOption {
        MS_PAINT_COLORS,
        MANDELBROT
    };

    // Image dimensions
    const int WIDTH = 1536;
    const int HEIGHT = 1536;

    // Color depth (8-bit per channel)
    const int COLOR_DEPTH = 256;

    // Maximum number of iterations for Mandelbrot algorithm
    const int MAX_ITERATIONS = 1000;

    // Default Constructor
    Texture();

    Texture(ImageOption option);

    // Destructor
    ~Texture();

    // Copy Constructor
    Texture(const Texture& other);

    // Assignment Operator
    Texture& operator=(const Texture& other);

    GLuint getTextureId();

    // Function to map a value from one range to another
    double map(const double& value, const double& in_min, const double& in_max, const double& out_min, const double& out_max);

    // Function to generate Mandelbrot fractal and store RGB values in the output array
    void generateMandelbrot(unsigned char* image);

    // Function to generate Mandelbrot fractal and store RGB values in the output array
    void generateMSPaintColors(_vec3<GLubyte>* pixelBuffer);

    static vec3 fetchFromSpectrum(const float& value);

private:

    GLuint textureId;

};


#endif //LIVEWALLPAPER05_TEXTURE_H