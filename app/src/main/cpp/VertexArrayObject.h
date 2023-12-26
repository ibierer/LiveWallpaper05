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

    VertexArrayObject(Vertex* vertices, const size_t& size);

    VertexArrayObject(VertexNormal* vertices, const size_t& size);

    // Copy constructor
    VertexArrayObject(const VertexArrayObject& other);

    // Assignment operator
    VertexArrayObject& operator=(const VertexArrayObject& other);

    ~VertexArrayObject();

private:

    GLuint mVAO;

    GLuint mVBO;

};


#endif //LIVEWALLPAPER05_VERTEXARRAYOBJECT_H
