//
// Created by Immanuel Bierer on 12/24/2023.
//

#include "SphereView.h"

struct object{
    VertexNormal* vertices;
    uvec3* indices;
    int numIndices;
};

object generateSoccerBallVertices(const float& radius) {
    const float goldenRatio = (1.0f + std::sqrt(5.0f)) / 2.0f;

    VertexNormal* vertices = (VertexNormal*)malloc(1000 * sizeof(VertexNormal));
    int numVertices = 0;

    vertices[numVertices++].v = vec3(0.0f, 0.0f, 1.0f);
    for(int i = 0; i < 6; i++){
        const float theta = M_PI * i / 3.0;
        const float phi = 1.0f / 9.0f * M_PI;
        const float sineOfPhi = sinf(phi);
        vertices[numVertices++].v = vec3(sineOfPhi * cosf(theta), sineOfPhi * sinf(theta), cosf(phi));
    }

    for(int i = 0; i < 6; i++){

    }

    // Set normals and set scale according to radius parameter.
    for(int i = 0; i < numVertices; i++) {
        vertices[i].n = vertices[i].v;
        vertices[i].v *= radius;
    }

    uint* indices = (uint*)malloc(1000 * sizeof(uvec3));

    int numIndices = 0;

    for(int i = 0; i < 6; i++){
        indices[numIndices++] = 0;
        indices[numIndices++] = i + 1;
        indices[numIndices++] = (i + 1) % 6 + 1;
    }

    return {
        (VertexNormal*)realloc(vertices, numVertices * sizeof(VertexNormal)),
        (uvec3*)realloc(indices, numVertices * sizeof(uvec3)),
        numIndices
    };
}

SphereView::SphereView() : View(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
}

SphereView::~SphereView(){
    glDeleteProgram(mProgram);
}

void SphereView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

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


    /*VertexNormal vertices[3] = {
            {vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)},
            {vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)},
            {vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)}
    };
    for(int i = 0; i < sizeof(vertices) / sizeof(VertexNormal); i++){
        vertices[i].n = normalize(cross(vertices[2].v - vertices[1].v, vertices[1].v - vertices[0].v));
    }*/
    object sphere = generateSoccerBallVertices(1.0f);
    /*uvec3 indices[1] = {
            uvec3(0, 1, 2)
    };*/
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&sphere.vertices[0].v);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&sphere.vertices[0].n);
    glDrawElements(GL_TRIANGLES, sphere.numIndices, GL_UNSIGNED_INT, sphere.indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    free(sphere.vertices);
    free(sphere.indices);
}
