//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "Cube.h"

Cube::Cube(const float& radius, const int& resolution) {

    const int numVertices = 18;
    this->numVertices = numVertices;
    vertices = (VertexNormal*)malloc(numVertices * sizeof(VertexNormal));

    // FRONT
    /*{{-0.5f, -0.5f,  0.5f}},
    {{ 0.5f, -0.5f,  0.5f}},
    {{-0.5f,  0.5f,  0.5f}},
    {{ 0.5f,  0.5f,  0.5f}},
    // BACK
    {{-0.5f, -0.5f, -0.5f}},
    {{-0.5f,  0.5f, -0.5f}},
    {{ 0.5f, -0.5f, -0.5f}},
    {{ 0.5f,  0.5f, -0.5f}},
    // LEFT
    {{-0.5f, -0.5f,  0.5f}},
    {{-0.5f,  0.5f,  0.5f}},
    {{-0.5f, -0.5f, -0.5f}},
    {{-0.5f,  0.5f, -0.5f}},
    // RIGHT
    {{ 0.5f, -0.5f, -0.5f}},
    {{ 0.5f,  0.5f, -0.5f}},
    {{ 0.5f, -0.5f,  0.5f}},
    {{ 0.5f,  0.5f,  0.5f}},
    // TOP
    {{-0.5f,  0.5f,  0.5f}},
    {{ 0.5f,  0.5f,  0.5f}},
    {{-0.5f,  0.5f, -0.5f}},
    {{ 0.5f,  0.5f, -0.5f}},
    // BOTTOM
    {{-0.5f, -0.5f,  0.5f}},
    {{-0.5f, -0.5f, -0.5f}},
    {{ 0.5f, -0.5f,  0.5f}},
    {{ 0.5f, -0.5f, -0.5f}}*/

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
        this->vertices[i] = vertices[i];
    }
}

Cube::Cube() {

}
