//
// Created by Immanuel Bierer on 12/11/2023.
//

#include "CubeMapView.h"

GLuint textureId;

GLubyte* cubemapPixelBuffers[6];

GLuint genCubeMap(GLint internalFormat, GLint param, GLsizei resolution) {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, resolution, resolution, 0, GL_RGB, GL_UNSIGNED_BYTE, cubemapPixelBuffers == NULL ? 0 : cubemapPixelBuffers[i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, param);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, param);
    return textureId;
}

GLuint createSimpleTextureCubemap() {
    GLsizei resolution = 256;
    for (int i = 0; i < 6; i++) {
        cubemapPixelBuffers[i] = (GLubyte*)malloc(3 * resolution * resolution * sizeof(unsigned char));
    }
    for (uint i = 0; i < resolution; i++) {
        for (uint j = 0; j < resolution; j++) {
            uint offset = 3 * (i * resolution + j);
            uint r = offset;
            uint g = offset + 1;
            uint b = offset + 2;
            cubemapPixelBuffers[0][r] = 255 - j;
            cubemapPixelBuffers[0][g] = 255 - i;
            cubemapPixelBuffers[0][b] = 255;
            cubemapPixelBuffers[1][r] = j;
            cubemapPixelBuffers[1][g] = 255 - i;
            cubemapPixelBuffers[1][b] = 0;
            cubemapPixelBuffers[2][r] = i;
            cubemapPixelBuffers[2][g] = 255;
            cubemapPixelBuffers[2][b] = j;
            cubemapPixelBuffers[3][r] = 255 - i;
            cubemapPixelBuffers[3][g] = 0;
            cubemapPixelBuffers[3][b] = j;
            cubemapPixelBuffers[4][r] = 255;
            cubemapPixelBuffers[4][g] = 255 - i;
            cubemapPixelBuffers[4][b] = j;
            cubemapPixelBuffers[5][r] = 0;
            cubemapPixelBuffers[5][g] = 255 - i;
            cubemapPixelBuffers[5][b] = 255 - j;
        }
    }
    return genCubeMap(GL_RGB, GL_LINEAR, resolution);
}

CubeMapView::CubeMapView() : Wallpaper(){
    mProgram = createProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    textureId = createSimpleTextureCubemap();
}

CubeMapView::~CubeMapView(){
    glDeleteProgram(mProgram);
}

void CubeMapView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
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
