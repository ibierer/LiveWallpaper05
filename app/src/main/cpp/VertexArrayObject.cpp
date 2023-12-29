//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() {

}

VertexArrayObject::VertexArrayObject(Vertex* const vertices, const int& count) {
    part1(vertices, count);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)offsetof(VertexNormal, v));
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArrayObject::VertexArrayObject(VertexNormal* const vertices, const int& count) {
    part1(vertices, count);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)offsetof(VertexNormal, v));
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)offsetof(VertexNormal, n));
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArrayObject::VertexArrayObject(VertexColor* const vertices, const int& count) {
    part1(vertices, count);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(COLOR_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor), (const GLvoid*)offsetof(VertexColor, v));
    glVertexAttribPointer(COLOR_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor), (const GLvoid*)offsetof(VertexColor, c));
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Copy constructor
VertexArrayObject::VertexArrayObject(const VertexArrayObject& other) {
    mVAO = other.mVAO;
    mVBO = other.mVBO;
    numVertices = other.numVertices;
}

// Assignment operator
VertexArrayObject& VertexArrayObject::operator=(const VertexArrayObject& other) {
    if (this != &other) {
        mVAO = other.mVAO;
        mVBO = other.mVBO;
        numVertices = other.numVertices;
    }
    return *this;
}

VertexArrayObject::~VertexArrayObject() {
    //glDeleteVertexArrays(1, &mVAO);
    //glDeleteBuffers(1, &mVBO);
}

template<class T>
void VertexArrayObject::part1(T* const vertices, const int& count) {
    numVertices = count;
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(T), vertices, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
}

GLuint VertexArrayObject::getArrayObjectId() {
    return mVAO;
}

GLuint VertexArrayObject::getBufferObjectId() {
    return mVBO;
}

VertexArrayObject::VertexArrayObject(const TriangleStripObject &triangleStripObject) {
    TriangleStripObject::AttributeType attributeType = triangleStripObject.getAttributeType();
    switch(attributeType){
        case TriangleStripObject::AttributeType::VERTEX:
            *this = VertexArrayObject(triangleStripObject.getVertices<Vertex>(), triangleStripObject.getNumVertices());
            break;
        case TriangleStripObject::AttributeType::VERTEX_NORMAL:
            *this = VertexArrayObject(triangleStripObject.getVertices<VertexNormal>(), triangleStripObject.getNumVertices());
            break;
        case TriangleStripObject::AttributeType::VERTEX_COLOR:
            *this = VertexArrayObject(triangleStripObject.getVertices<VertexColor>(), triangleStripObject.getNumVertices());
            break;
    }
}

void VertexArrayObject::draw(){
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);
    glBindVertexArray(0);
}
