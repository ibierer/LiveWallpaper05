//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMapView.h"

SphereMapView::SphereMapView() : Wallpaper(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    sphereMap = SphereMap();
}

SphereMapView::~SphereMapView(){
    glDeleteProgram(mProgram);
}

void SphereMapView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
    //glActiveTexture(GL_TEXTURE0);

    Vertex vertices[3] = {
            {vec3(-1.0f, -1.0f, 0.999f)},
            {vec3( 3.0f, -1.0f, 0.999f)},
            {vec3(-1.0f,  3.0f, 0.999f)}
    };
    uvec3 indices[1] = {
            uvec3(0, 1, 2)
    };
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)&vertices[0].v);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
}