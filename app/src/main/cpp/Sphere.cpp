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

    this->radius = radius;
    vertices = (PositionXYZNormalXYZ*)malloc(2 * horizontalSegments * (verticalSegments + 1) * sizeof(PositionXYZNormalXYZ));
    numVertices = 0;
    attributeType = POSITION_XYZ_NORMAL_XYZ;

    for(int i = 0; i < horizontalSegments; i++) {
        for (int j = 0; j <= verticalSegments; j++) {
            theta = twoPi * j / verticalSegments;
            sineOfPhi = sin(phi = twoPi * i / verticalSegments);
            ((PositionXYZNormalXYZ*)vertices)[numVertices++].p = vec3(sineOfPhi * cos(theta), sineOfPhi * sin(theta), cos(phi));
            sineOfPhi = sin(phi = twoPi * (i + 1) / verticalSegments);
            ((PositionXYZNormalXYZ*)vertices)[numVertices++].p = vec3(sineOfPhi * cos(theta), sineOfPhi * sin(theta), cos(phi));
        }
    }

    // Set normals and set scale according to radius parameter.
    for(int i = 0; i < getNumVertices(); i++) {
        ((PositionXYZNormalXYZ*)vertices)[i].n = ((PositionXYZNormalXYZ*)vertices)[i].p;
        ((PositionXYZNormalXYZ*)vertices)[i].p *= radius;
    }
}

Sphere::Sphere() {

}

float Sphere::getRadius() {
    return radius;
}
