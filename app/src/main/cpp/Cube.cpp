//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "Cube.h"

Cube::Cube(const float& size) {

    const int numVertices = 18;
    const float radius = size / 2;
    this->numVertices = numVertices;
    vertices = (Vertex*)malloc(numVertices * sizeof(Vertex));

    Vertex vertices[] = {
            Vertex(vec3(-radius, -radius,  radius)),
            Vertex(vec3( radius, -radius,  radius)),
            Vertex(vec3(-radius,  radius,  radius)),
            Vertex(vec3( radius,  radius,  radius)),
            Vertex(vec3(-radius,  radius, -radius)),
            Vertex(vec3( radius,  radius, -radius)),
            Vertex(vec3(-radius, -radius, -radius)),
            Vertex(vec3( radius, -radius, -radius)),
            Vertex(vec3( radius, -radius, -radius)),
            Vertex(vec3( radius,  radius, -radius)),
            Vertex(vec3( radius,  radius, -radius)),
            Vertex(vec3( radius,  radius,  radius)),
            Vertex(vec3( radius, -radius, -radius)),
            Vertex(vec3( radius, -radius,  radius)),
            Vertex(vec3(-radius, -radius, -radius)),
            Vertex(vec3(-radius, -radius,  radius)),
            Vertex(vec3(-radius,  radius, -radius)),
            Vertex(vec3(-radius,  radius,  radius)),
    };

    for(int i = 0; i < numVertices; i++) {
        ((Vertex*)this->vertices)[i] = vertices[i];
    }
}

Cube::Cube() {

}
