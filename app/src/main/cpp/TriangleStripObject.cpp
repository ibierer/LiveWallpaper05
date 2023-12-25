//
// Created by Immanuel Bierer on 12/25/2023.
//

#include "TriangleStripObject.h"

TriangleStripObject::TriangleStripObject() {

}

TriangleStripObject::TriangleStripObject(const TriangleStripObject& other) : numVertices(other.numVertices) {
    vertices = (VertexNormal*)malloc(numVertices * sizeof(VertexNormal));
    // Copy the contents of the other object's vertices array
    std::copy(other.vertices, other.vertices + numVertices, vertices);
}

TriangleStripObject& TriangleStripObject::operator=(TriangleStripObject other) {
    // Swap the content of 'this' and 'other'
    std::swap(numVertices, other.numVertices);
    std::swap(vertices, other.vertices);
    return *this;
}

TriangleStripObject::~TriangleStripObject() {
    free(vertices);
};

int TriangleStripObject::getNumVertices() {
    return numVertices;
};

VertexNormal* TriangleStripObject::getVertices() {
    return vertices;
}
