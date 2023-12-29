//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_POSITIONXYZNORMALXYZ_H
#define LIVEWALLPAPER05_POSITIONXYZNORMALXYZ_H


#include "VertexAttributesArray.h"

#define NORMAL_ATTRIBUTE_LOCATION 1

class PositionXYZNormalXYZ : public PositionXYZ {
public:

    vec3 n;

    PositionXYZNormalXYZ();

    PositionXYZNormalXYZ(const vec3& position, const vec3& normal);

};


#endif //LIVEWALLPAPER05_POSITIONXYZNORMALXYZ_H
