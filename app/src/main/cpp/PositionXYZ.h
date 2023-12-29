//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_POSITIONXYZ_H
#define LIVEWALLPAPER05_POSITIONXYZ_H


#include "VertexAttributesArray.h"

class PositionXYZ : public VertexAttributesArray {
public:

    vec3 p;

    PositionXYZ();

    PositionXYZ(const vec3& position);

};


#endif //LIVEWALLPAPER05_POSITIONXYZ_H
