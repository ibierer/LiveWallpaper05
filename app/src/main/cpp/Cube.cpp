//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "Cube.h"

Cube::Cube(const float& radius, const int& resolution) {

    const int numVertices = 18;
    this->numVertices = numVertices;
    vertices = (VertexNormal*)malloc(numVertices * sizeof(VertexNormal));

    VertexNormal vertices[] = {
            VertexNormal(vec3(-radius, -radius,  radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, -radius,  radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3(-radius,  radius,  radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius,  radius,  radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3(-radius,  radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius,  radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3(-radius, -radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, -radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, -radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, radius, radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, -radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( radius, -radius, radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( -radius, -radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( -radius, -radius, radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( -radius, radius, -radius), vec3(0.0f, 0.0f, 1.0f)),
            VertexNormal(vec3( -radius, radius, radius), vec3(0.0f, 0.0f, 1.0f)),
    };

    for(int i = 0; i < numVertices; i++) {
        ((VertexNormal*)this->vertices)[i] = vertices[i];
    }
}

Cube::Cube() {

}
