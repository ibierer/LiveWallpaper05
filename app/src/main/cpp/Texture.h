//
// Created by Immanuel Bierer on 12/14/2023.
//

#ifndef LIVEWALLPAPER05_TEXTURE_H
#define LIVEWALLPAPER05_TEXTURE_H


#include <complex>

class Texture {
public:

    Texture(const GLuint &texture);

    enum ImageOption {
        MS_PAINT_COLORS,
        MANDELBROT
    };

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

    static vec3 fetchFromSpectrum(const float& value);

    static GLuint generateTexture(const int& width, const int& height);

    int getWidth();

    int getHeight();

protected:

    void generateTexture(const ImageOption& option);

    GLuint textureId;

private:

    int width;

    int height;

    void generateMandelbrot(unsigned char* image, const int& WIDTH, const int& HEIGHT);

    void generateMSPaintColors(_vec3<GLubyte>* pixelBuffer, const int& WIDTH, const int& HEIGHT);

};


#endif //LIVEWALLPAPER05_TEXTURE_H