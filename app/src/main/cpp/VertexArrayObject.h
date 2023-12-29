//
// Created by Immanuel Bierer on 12/26/2023.
//

#ifndef LIVEWALLPAPER05_VERTEXARRAYOBJECT_H
#define LIVEWALLPAPER05_VERTEXARRAYOBJECT_H


class VertexArrayObject {
public:

    GLuint getArrayObjectId();

    GLuint getBufferObjectId();

    VertexArrayObject();

    VertexArrayObject(Vertex* vertices, const int& count);

    VertexArrayObject(VertexNormal* vertices, const int& count);

    VertexArrayObject(const TriangleStripObject& triangleStripObject);

    // Copy constructor
    VertexArrayObject(const VertexArrayObject& other);

    // Assignment operator
    VertexArrayObject& operator=(const VertexArrayObject& other);

    ~VertexArrayObject();

    void draw();

private:

    GLuint mVAO;

    GLuint mVBO;

    int numVertices;
};


#endif //LIVEWALLPAPER05_VERTEXARRAYOBJECT_H
