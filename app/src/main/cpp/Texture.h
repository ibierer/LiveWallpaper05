//
// Created by Immanuel Bierer on 12/14/2023.
//

#ifndef LIVEWALLPAPER05_TEXTURE_H
#define LIVEWALLPAPER05_TEXTURE_H


class Texture {
public:

    enum imageOption {
        MS_PAINT_COLORS,
        MANDLEBROT
    };

    // Default Constructor
    Texture();

    Texture(imageOption option);

    // Destructor
    ~Texture();

    // Copy Constructor
    Texture(const Texture& other);

    // Assignment Operator
    Texture& operator=(const Texture& other);

    GLuint getTextureId();

    // Function to generate MS Paint color picker image and store RGB values in the output array
    void generateMSPaintColors(_vec3<GLubyte>* pixelBuffer);

    // Function to generate Mandelbrot fractal and store RGB values in the output array
    void generateMandelbrot(unsigned char* image);

protected:

    // Image dimensions
    const int WIDTH = 1536;
    const int HEIGHT = 1536;

    // Color depth (8-bit per channel)
    const int COLOR_DEPTH = 256;

    // Maximum number of iterations for Mandelbrot algorithm
    const int MAX_ITERATIONS = 1000;

    // Function to map a value from one range to another
    double map(double value, double in_min, double in_max, double out_min, double out_max);

private:

    GLuint textureId;

};


#endif //LIVEWALLPAPER05_TEXTURE_H
