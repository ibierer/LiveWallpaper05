//
// Created by Immanuel Bierer on 12/26/2023.
//

#ifndef LIVEWALLPAPER05_CUBE_H
#define LIVEWALLPAPER05_CUBE_H


#include "TriangleStripObject.h"

class Cube : public TriangleStripObject {
public:

    enum ColorOption {
        SOLID,
        RGB_CUBE
    };

    Cube();

    Cube(const float& size, const ColorOption& colorOption);

    static const Vertex box[24];

};


#endif //LIVEWALLPAPER05_CUBE_H
