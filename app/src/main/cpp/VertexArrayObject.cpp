//
// Created by Immanuel Bierer on 12/26/2023.
//

#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() {

}

VertexArrayObject::VertexArrayObject(const PositionXYZ* const vertices, const int& count) {
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(PositionXYZ), vertices, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZ), (const GLvoid*)offsetof(PositionXYZ, p));
    glBindVertexArray(0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    numVertices = count;
}

VertexArrayObject::VertexArrayObject(const PositionXYZNormalXYZ* const vertices, const int& count) {
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(PositionXYZNormalXYZ), vertices, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)offsetof(PositionXYZNormalXYZ, p));
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)offsetof(PositionXYZNormalXYZ, n));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    numVertices = count;
}

VertexArrayObject::VertexArrayObject(const PositionXYZColorRGB* const vertices, const int& count) {
    glGenBuffers(1, &mVBO);
    glGenVertexArrays(1, &mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(PositionXYZColorRGB), vertices, GL_STATIC_DRAW);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(COLOR_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZColorRGB), (const GLvoid*)offsetof(PositionXYZColorRGB, p));
    glVertexAttribPointer(COLOR_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZColorRGB), (const GLvoid*)offsetof(PositionXYZColorRGB, c));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE_LOCATION);
    numVertices = count;
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

GLuint VertexArrayObject::getArrayObjectId() {
    return mVAO;
}

GLuint VertexArrayObject::getBufferObjectId() {
    return mVBO;
}

VertexArrayObject::VertexArrayObject(const TriangleStripObject &triangleStripObject) {
    TriangleStripObject::AttributeType attributeType = triangleStripObject.getAttributeType();
    switch(attributeType){
        case TriangleStripObject::AttributeType::POSITION_XYZ:
            *this = VertexArrayObject(triangleStripObject.getVertices<PositionXYZ>(), triangleStripObject.getNumVertices());
            break;
        case TriangleStripObject::AttributeType::POSITION_XYZ_NORMAL_XYZ:
            *this = VertexArrayObject(triangleStripObject.getVertices<PositionXYZNormalXYZ>(), triangleStripObject.getNumVertices());
            break;
        case TriangleStripObject::AttributeType::POSITION_XYZ_COLOR_RGB:
            *this = VertexArrayObject(triangleStripObject.getVertices<PositionXYZColorRGB>(), triangleStripObject.getNumVertices());
            break;
    }
}

void VertexArrayObject::drawArrays(){
    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);
    glBindVertexArray(0);
}

void VertexArrayObject::drawArraysInstanced(const int& instanceCount){
    glBindVertexArray(mVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, numVertices, instanceCount);
    glBindVertexArray(0);
}

int VertexArrayObject::getNumVertices() {
    return numVertices;
}
