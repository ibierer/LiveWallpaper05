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
            attributeType = POSITION_XYZ;
            PositionXYZ vertices[numVertices] = {
                    PositionXYZ(vec3(-radius, -radius, radius)),
                    PositionXYZ(vec3(radius, -radius, radius)),
                    PositionXYZ(vec3(-radius, radius, radius)),
                    PositionXYZ(vec3(radius, radius, radius)),
                    PositionXYZ(vec3(-radius, radius, -radius)),
                    PositionXYZ(vec3(radius, radius, -radius)),
                    PositionXYZ(vec3(-radius, -radius, -radius)),
                    PositionXYZ(vec3(radius, -radius, -radius)),
                    PositionXYZ(vec3(radius, -radius, -radius)),
                    PositionXYZ(vec3(radius, radius, -radius)),
                    PositionXYZ(vec3(radius, radius, -radius)),
                    PositionXYZ(vec3(radius, radius, radius)),
                    PositionXYZ(vec3(radius, -radius, -radius)),
                    PositionXYZ(vec3(radius, -radius, radius)),
                    PositionXYZ(vec3(-radius, -radius, -radius)),
                    PositionXYZ(vec3(-radius, -radius, radius)),
                    PositionXYZ(vec3(-radius, radius, -radius)),
                    PositionXYZ(vec3(-radius, radius, radius))
            };
            this->vertices = (PositionXYZ*)malloc(numVertices * sizeof(PositionXYZ));
            for(int i = 0; i < numVertices; i++) {
                ((PositionXYZ*)this->vertices)[i] = vertices[i];
            }
            break;}
        case RGB_CUBE:{
            attributeType = POSITION_XYZ_COLOR_RGB;
            PositionXYZColorRGB vertices[numVertices] = {
                    PositionXYZColorRGB(vec3(-radius, -radius, radius), vec3(0.0f, 0.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(radius, -radius, radius), vec3(1.0f, 0.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(-radius, radius, radius), vec3(0.0f, 1.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(radius, radius, radius), vec3(1.0f, 1.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(-radius, radius, -radius), vec3(0.0f, 1.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, radius, -radius), vec3(1.0f, 1.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(-radius, -radius, -radius), vec3(0.0f, 0.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, -radius, -radius), vec3(1.0f, 0.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, -radius, -radius), vec3(1.0f, 0.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, radius, -radius), vec3(1.0f, 1.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, radius, -radius), vec3(1.0f, 1.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, radius, radius), vec3(1.0f, 1.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(radius, -radius, -radius), vec3(1.0f, 0.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(radius, -radius, radius), vec3(1.0f, 0.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(-radius, -radius, -radius), vec3(0.0f, 0.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(-radius, -radius, radius), vec3(0.0f, 0.0f, 1.0f)),
                    PositionXYZColorRGB(vec3(-radius, radius, -radius), vec3(0.0f, 1.0f, 0.0f)),
                    PositionXYZColorRGB(vec3(-radius, radius, radius), vec3(0.0f, 1.0f, 1.0f))
            };
            this->vertices = (PositionXYZColorRGB*)malloc(numVertices * sizeof(PositionXYZColorRGB));
            for(int i = 0; i < numVertices; i++) {
                ((PositionXYZColorRGB*)this->vertices)[i] = vertices[i];
            }
            break;}
    }
}

Cube::Cube() {

}
