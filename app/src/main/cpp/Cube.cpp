//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "Cube.h"

Cube::Cube(const float& size, const ColorOption& colorOption) {
    const int numVertices = 18;
    const float radius = size / 2;
    this->numVertices = numVertices;
    switch(colorOption){
        case SOLID:{
            attributeType = VERTEX;
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
                    Vertex(vec3(-radius,  radius,  radius))
            };
            this->vertices = (Vertex*)malloc(numVertices * sizeof(Vertex));
            for(int i = 0; i < numVertices; i++) {
                ((Vertex*)this->vertices)[i] = vertices[i];
            }
            break;}
        case RGB_CUBE:{
            attributeType = VERTEX_NORMAL;
            VertexNormal vertices[18] = {
                    VertexNormal(vec3(-radius, -radius,  radius), vec3(0.0f, 0.0f, 1.0f)),
                    VertexNormal(vec3( radius, -radius,  radius), vec3(1.0f, 0.0f, 1.0f)),
                    VertexNormal(vec3(-radius,  radius,  radius), vec3(0.0f, 1.0f, 1.0f)),
                    VertexNormal(vec3( radius,  radius,  radius), vec3(1.0f, 1.0f, 1.0f)),
                    VertexNormal(vec3(-radius,  radius, -radius), vec3(0.0f, 1.0f, 0.0f)),
                    VertexNormal(vec3( radius,  radius, -radius), vec3(1.0f, 1.0f, 0.0f)),
                    VertexNormal(vec3(-radius, -radius, -radius), vec3(0.0f, 0.0f, 0.0f)),
                    VertexNormal(vec3( radius, -radius, -radius), vec3(1.0f, 0.0f, 0.0f)),
                    VertexNormal(vec3( radius, -radius, -radius), vec3(1.0f, 0.0f, 0.0f)),
                    VertexNormal(vec3( radius,  radius, -radius), vec3(1.0f, 1.0f, 0.0f)),
                    VertexNormal(vec3( radius,  radius, -radius), vec3(1.0f, 1.0f, 0.0f)),
                    VertexNormal(vec3( radius,  radius,  radius), vec3(1.0f, 1.0f, 1.0f)),
                    VertexNormal(vec3( radius, -radius, -radius), vec3(1.0f, 0.0f, 0.0f)),
                    VertexNormal(vec3( radius, -radius,  radius), vec3(1.0f, 0.0f, 1.0f)),
                    VertexNormal(vec3(-radius, -radius, -radius), vec3(0.0f, 0.0f, 0.0f)),
                    VertexNormal(vec3(-radius, -radius,  radius), vec3(0.0f, 0.0f, 1.0f)),
                    VertexNormal(vec3(-radius,  radius, -radius), vec3(0.0f, 1.0f, 0.0f)),
                    VertexNormal(vec3(-radius,  radius,  radius), vec3(0.0f, 1.0f, 1.0f))
            };
            this->vertices = (VertexNormal*)malloc(numVertices * sizeof(VertexNormal));
            for(int i = 0; i < numVertices; i++) {
                ((VertexNormal*)this->vertices)[i] = vertices[i];
            }
            break;}
    }
}

Cube::Cube() {

}
