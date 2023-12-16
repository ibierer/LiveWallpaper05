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

    //int width;

    //int height;

    // Default Constructor
    Texture();

    Texture(const ImageOption& option);

    // Destructor
    ~Texture();

    // Copy Constructor
    Texture(const Texture& other);

    // Assignment Operator
    Texture& operator=(const Texture& other);

    GLuint getTextureId();

    // Function to generate Mandelbrot fractal and store RGB values in the output array
    void generateMandelbrot(unsigned char* image, const int& WIDTH, const int& HEIGHT);

    // Function to generate Mandelbrot fractal and store RGB values in the output array
    void generateMSPaintColors(_vec3<GLubyte>* pixelBuffer, const int& WIDTH, const int& HEIGHT);

    static vec3 fetchFromSpectrum(const float& value);

protected:

    void generateTexture(const ImageOption& option);

    void generateTexture2(const ImageOption& option);

    GLuint textureId;

private:

};


#endif //LIVEWALLPAPER05_TEXTURE_H