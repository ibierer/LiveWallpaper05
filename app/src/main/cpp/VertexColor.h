//
// Created by Immanuel Bierer on 12/28/2023.
//

#ifndef LIVEWALLPAPER05_VERTEXCOLOR_H
#define LIVEWALLPAPER05_VERTEXCOLOR_H


#include "Attributes.h"

#define COLOR_ATTRIBUTE_LOCATION 2

class VertexColor : public Attributes {
public:

    VertexColor();

    VertexColor(const vec3& position, const vec3& color);

    vec3 c;

};


#endif //LIVEWALLPAPER05_VERTEXCOLOR_H
