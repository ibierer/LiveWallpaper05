//
// Created by Immanuel Bierer on 12/25/2023.
//

#ifndef LIVEWALLPAPER05_SPHERE_H
#define LIVEWALLPAPER05_SPHERE_H


#include "TriangleStripObject.h"

class Sphere : public TriangleStripObject {
public:

    Sphere();

    Sphere(const float& radius, const int& resolution);

};


#endif //LIVEWALLPAPER05_SPHERE_H
