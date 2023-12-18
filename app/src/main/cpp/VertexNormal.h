//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_VERTEXNORMAL_H
#define LIVEWALLPAPER05_VERTEXNORMAL_H


#include "Attributes.h"

#define NORMAL_ATTRIBUTE_LOCATION 1

class VertexNormal : public Attributes {
public:
    vec3 n;
    VertexNormal(const vec3& position, const vec3& normal);
};


#endif //LIVEWALLPAPER05_VERTEXNORMAL_H
