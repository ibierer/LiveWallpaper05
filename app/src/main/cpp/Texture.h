//
// Created by Immanuel Bierer on 12/14/2023.
//

#ifndef LIVEWALLPAPER05_TEXTURE_H
#define LIVEWALLPAPER05_TEXTURE_H


#include <complex>
#include "FBO.h"

class Texture {
public:

    enum DefaultImages {
        MS_PAINT_COLORS,
        MANDELBROT,
        RGB_CUBE
    };

    static const string FETCH_FROM_SPECTRUM_FUNCTION;

    Texture(const GLuint &texture, const int& w, const int& h);

    Texture(const GLint& internalFormat, const int &width, const int &height, const float *pixelBuffer, const GLenum param);

    // Default Constructor
    Texture();

    Texture(const DefaultImages &option, const int &w, const int &h, View *view);

    // Destructor
    ~Texture();

    // Copy Constructor
    Texture(const Texture& other);

    // Assignment Operator
    Texture& operator=(const Texture& other);

    GLuint getTextureId();

    static vec3 fetchFromSpectrum(const float& value);

    int getWidth();

    int getHeight();

    static Texture& dynamicallyGenerateMandelbrotWithVertexShader(FBO& fbo, View* view);

    static Texture& staticallyGenerateMandelbrotWithVertexShader(const Texture& texture, View* view);

protected:

    void generateTexture(const DefaultImages& option, const int& w, const int& h, View* view);

    GLuint textureId;

    int width;

    int height;

private:

    void generateMandelbrot(unsigned char* image, const int& WIDTH, const int& HEIGHT);

    void generateMSPaintColors(_vec3<GLubyte>* pixelBuffer, const int& WIDTH, const int& HEIGHT);

};


#endif //LIVEWALLPAPER05_TEXTURE_H