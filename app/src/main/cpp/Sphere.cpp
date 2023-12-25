//
// Created by Immanuel Bierer on 12/25/2023.
//

#include "Sphere.h"

Sphere::Sphere(const float& radius, const int& resolution) {
    const double twoPi = 2.0 * M_PI;
    const int horizontalSegments = resolution;
    const int verticalSegments = 2 * horizontalSegments;
    float theta;
    float phi;
    float sineOfPhi;

    vertices = (VertexNormal*)malloc(2 * horizontalSegments * (verticalSegments + 1) * sizeof(VertexNormal));
    numVertices = 0;

    for(int i = 0; i < horizontalSegments; i++) {
        for (int j = 0; j <= verticalSegments; j++) {
            theta = twoPi * j / verticalSegments;
            sineOfPhi = sinf(phi = twoPi * i / verticalSegments);
            vertices[numVertices++].v = vec3(sineOfPhi * cosf(theta), sineOfPhi * sinf(theta), cosf(phi));
            sineOfPhi = sinf(phi = twoPi * (i + 1) / verticalSegments);
            vertices[numVertices++].v = vec3(sineOfPhi * cosf(theta), sineOfPhi * sinf(theta), cosf(phi));
        }
    }

    // Set normals and set scale according to radius parameter.
    for(int i = 0; i < getNumVertices(); i++) {
        vertices[i].n = vertices[i].v;
        vertices[i].v *= radius;
    }
}

Sphere::Sphere() {

}
