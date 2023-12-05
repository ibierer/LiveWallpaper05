//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "VertexNormal.h"

VertexNormal::VertexNormal(const vec3& position, const vec3& normal) {
    this->v = position;
    this->n = normal;
}
