//
// Created by Immanuel Bierer on 12/25/2023.
//

#include "TriangleStripObject.h"

TriangleStripObject::TriangleStripObject() {

}

TriangleStripObject::TriangleStripObject(const TriangleStripObject& other) {
    numVertices = other.numVertices;
    attributeType = other.attributeType;
    switch(attributeType){
        case VERTEX:
            vertices = (Vertex*)malloc(numVertices * sizeof(Vertex));
            // Copy the contents of the other object's environmentTriangleVertices array
            std::copy((Vertex*)other.vertices, ((Vertex*)other.vertices) + numVertices, (Vertex*)vertices);
            break;
        case VERTEX_NORMAL:
            vertices = (VertexNormal*)malloc(numVertices * sizeof(VertexNormal));
            // Copy the contents of the other object's environmentTriangleVertices array
            std::copy((VertexNormal*)other.vertices, ((VertexNormal*)other.vertices) + numVertices, (VertexNormal*)vertices);
            break;
        case VERTEX_COLOR:
            vertices = (VertexColor*)malloc(numVertices * sizeof(VertexColor));
            // Copy the contents of the other object's environmentTriangleVertices array
            std::copy((VertexColor*)other.vertices, ((VertexColor*)other.vertices) + numVertices, (VertexColor*)vertices);
            break;
    }
}

TriangleStripObject& TriangleStripObject::operator=(TriangleStripObject other) {
    if (this != &other) {
        numVertices = other.numVertices;
        attributeType = other.attributeType;
        switch (attributeType) {
            case VERTEX:
                vertices = (Vertex*) malloc(numVertices * sizeof(Vertex));
                // Copy the contents of the other object's environmentTriangleVertices array
                std::copy((Vertex*) other.vertices, ((Vertex*) other.vertices) + numVertices, (Vertex*) vertices);
                break;
            case VERTEX_NORMAL:
                vertices = (VertexNormal*) malloc(numVertices * sizeof(VertexNormal));
                // Copy the contents of the other object's environmentTriangleVertices array
                std::copy((VertexNormal*) other.vertices, ((VertexNormal*) other.vertices) + numVertices, (VertexNormal*) vertices);
                break;
            case VERTEX_COLOR:
                vertices = (VertexColor*) malloc(numVertices * sizeof(VertexColor));
                // Copy the contents of the other object's environmentTriangleVertices array
                std::copy((VertexColor*) other.vertices, ((VertexColor*) other.vertices) + numVertices, (VertexColor*) vertices);
                break;
        }
    }
    return *this;
}

TriangleStripObject::~TriangleStripObject() {
    free(vertices);
};

int TriangleStripObject::getNumVertices() const {
    return numVertices;
}

TriangleStripObject::AttributeType TriangleStripObject::getAttributeType() const {
    return attributeType;
};

template<class T>
T* TriangleStripObject::getVertices() const {
    return (T*)vertices;
}
