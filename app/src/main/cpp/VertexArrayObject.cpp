//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() {

}

VertexArrayObject::VertexArrayObject(Vertex* vertices, const size_t& size) {
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, v));
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArrayObject::VertexArrayObject(VertexNormal* vertices, const size_t& size) {
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)offsetof(VertexNormal, v));
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)offsetof(VertexNormal, n));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
}

// Copy constructor
VertexArrayObject::VertexArrayObject(const VertexArrayObject& other) {
    mVAO = other.mVAO;
    mVBO = other.mVBO;
}

// Assignment operator
VertexArrayObject& VertexArrayObject::operator=(const VertexArrayObject& other) {
    if (this != &other) {
        mVAO = other.mVAO;
        mVBO = other.mVBO;
    }
    return *this;
}

VertexArrayObject::~VertexArrayObject() {
    //glDeleteVertexArrays(1, &mVAO);
    //glDeleteBuffers(1, &mVBO);
}

GLuint VertexArrayObject::getArrayObjectId() {
    return mVAO;
}

GLuint VertexArrayObject::getBufferObjectId() {
    return mVBO;
}

VertexArrayObject::VertexArrayObject(const TriangleStripObject &triangleStripObject) {
    switch(triangleStripObject.getAttributeType()){
        case TriangleStripObject::AttributeType::VERTEX:
            *this = VertexArrayObject(triangleStripObject.getVertices<Vertex>(), triangleStripObject.getNumVertices() * sizeof(Vertex));
            break;
        case TriangleStripObject::AttributeType::VERTEX_NORMAL:
            *this = VertexArrayObject(triangleStripObject.getVertices<VertexNormal>(), triangleStripObject.getNumVertices() * sizeof(VertexNormal));
            break;
    }
}
