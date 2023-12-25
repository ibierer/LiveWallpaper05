//
// Created by Immanuel Bierer on 12/25/2023.
//

#include "Sphere.h"

Sphere::Sphere(const float& radius, const int& resolution) {
    const double twoPi = 2.0 * M_PI;
    const int horizontalSegments = resolution;
    const int verticalSegments = 2 * horizontalSegments;
    double theta;
    double phi;
    double sineOfPhi;

    vertices = (VertexNormal*)malloc(2 * horizontalSegments * (verticalSegments + 1) * sizeof(VertexNormal));
    numVertices = 0;

    for(int i = 0; i < horizontalSegments; i++) {
        for (int j = 0; j <= verticalSegments; j++) {
            theta = twoPi * j / verticalSegments;
            sineOfPhi = sin(phi = twoPi * i / verticalSegments);
            vertices[numVertices++].v = vec3(sineOfPhi * cos(theta), sineOfPhi * sin(theta), cos(phi));
            sineOfPhi = sin(phi = twoPi * (i + 1) / verticalSegments);
            vertices[numVertices++].v = vec3(sineOfPhi * cos(theta), sineOfPhi * sin(theta), cos(phi));
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
