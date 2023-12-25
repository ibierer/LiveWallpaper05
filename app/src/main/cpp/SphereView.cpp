//
// Created by Immanuel Bierer on 12/24/2023.
//

#include "SphereView.h"

class Sphere{
public:
    static object generateSphere(const float& radius, const int& resolution);
};

object Sphere::generateSphere(const float& radius, const int& resolution){
    const double twoPi = 2.0 * M_PI;
    const int horizontalSegments = resolution;
    const int verticalSegments = 2 * horizontalSegments;
    float theta;
    float phi;
    float sineOfPhi;

    object sphere;

    sphere.vertices = (VertexNormal*)malloc(2 * horizontalSegments * (verticalSegments + 1) * sizeof(VertexNormal));
    sphere.numVertices = 0;

    for(int i = 0; i < horizontalSegments; i++) {
        for (int j = 0; j <= verticalSegments; j++) {
            theta = twoPi * j / verticalSegments;
            sineOfPhi = sinf(phi = twoPi * i / verticalSegments);
            sphere.vertices[sphere.numVertices++].v = vec3(sineOfPhi * cosf(theta), sineOfPhi * sinf(theta), cosf(phi));
            sineOfPhi = sinf(phi = twoPi * (i + 1) / verticalSegments);
            sphere.vertices[sphere.numVertices++].v = vec3(sineOfPhi * cosf(theta), sineOfPhi * sinf(theta), cosf(phi));
        }
    }

    // Set normals and set scale according to radius parameter.
    for(int i = 0; i < sphere.numVertices; i++) {
        sphere.vertices[i].n = sphere.vertices[i].v;
        sphere.vertices[i].v *= radius;
    }

    return sphere;
}

SphereView::SphereView() : View() {
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    sphere = Sphere::generateSphere(1.0f, 100);
}

SphereView::~SphereView(){
    glDeleteProgram(mProgram);
    free(sphere.vertices);
}

void SphereView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * (zoom - 1.0f)));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&sphere.vertices[0].v);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&sphere.vertices[0].n);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere.numVertices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
}
