//
// Created by Immanuel Bierer on 12/26/2023.
//

#ifndef LIVEWALLPAPER05_VERTEXARRAYOBJECT_H
#define LIVEWALLPAPER05_VERTEXARRAYOBJECT_H


#include <GLES3/gl32.h>

class VertexArrayObject {
public:

    GLuint getArrayObjectId();

    GLuint getBufferObjectId();

    int getNumVertices();

    VertexArrayObject();

    VertexArrayObject(const PositionXYZ* vertices, const int& count);

    VertexArrayObject(const PositionXYZNormalXYZ* vertices, const int& count);

    VertexArrayObject(const PositionXYZColorRGB* vertices, const int &count);

    VertexArrayObject(const TriangleStripObject& triangleStripObject);

    // Copy constructor
    VertexArrayObject(const VertexArrayObject& other);

    // Assignment operator
    VertexArrayObject& operator=(const VertexArrayObject& other);

    ~VertexArrayObject();

    void drawArrays();

    void drawArraysInstanced(const int& instanceCount);

private:

    GLuint mVAO;

    GLuint mVBO;

    int numVertices;

};


#endif //LIVEWALLPAPER05_VERTEXARRAYOBJECT_H
