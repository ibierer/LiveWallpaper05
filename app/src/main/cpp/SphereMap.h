//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREMAP_H
#define LIVEWALLPAPER05_SPHEREMAP_H


class SphereMap : public EnvironmentMap {
public:

    // Default Constructor
    SphereMap();

    SphereMap(Texture::imageOption option);

    // Destructor
    ~SphereMap();

    // Copy Constructor
    SphereMap(const SphereMap& other);

    // Assignment Operator
    SphereMap& operator=(const SphereMap& other);

    GLuint getTextureId();

    // Function to generate Mandelbrot fractal and store RGB values in the output array
    void generateMandelbrot(unsigned char* image);

private:

    GLuint textureId;

};


#endif //LIVEWALLPAPER05_SPHEREMAP_H
