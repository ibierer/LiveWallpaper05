//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_VERTEXNORMAL_H
#define LIVEWALLPAPER05_VERTEXNORMAL_H


#include "Attributes.h"

class VertexNormal : public Attributes {
public:
    vec3 n;
    VertexNormal(const vec3& position, const vec3& normal);
};


#endif //LIVEWALLPAPER05_VERTEXNORMAL_H
