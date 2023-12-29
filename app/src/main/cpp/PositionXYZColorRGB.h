//
// Created by Immanuel Bierer on 12/28/2023.
//

#ifndef LIVEWALLPAPER05_POSITIONXYZCOLORRGB_H
#define LIVEWALLPAPER05_POSITIONXYZCOLORRGB_H


#include "VertexAttributesArray.h"

#define COLOR_ATTRIBUTE_LOCATION 2

class PositionXYZColorRGB : public PositionXYZ {
public:

    vec3 c;

    PositionXYZColorRGB();

    PositionXYZColorRGB(const vec3& position, const vec3& color);

};


#endif //LIVEWALLPAPER05_POSITIONXYZCOLORRGB_H
