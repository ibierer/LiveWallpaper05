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
        case POSITION_XYZ:
            vertices = (PositionXYZ*)malloc(numVertices * sizeof(PositionXYZ));
            // Copy the contents of the other object's environmentTriangleVertices array
            std::copy((PositionXYZ*)other.vertices, ((PositionXYZ*)other.vertices) + numVertices, (PositionXYZ*)vertices);
            break;
        case POSITION_XYZ_NORMAL_XYZ:
            vertices = (PositionXYZNormalXYZ*)malloc(numVertices * sizeof(PositionXYZNormalXYZ));
            // Copy the contents of the other object's environmentTriangleVertices array
            std::copy((PositionXYZNormalXYZ*)other.vertices, ((PositionXYZNormalXYZ*)other.vertices) + numVertices, (PositionXYZNormalXYZ*)vertices);
            break;
        case POSITION_XYZ_COLOR_RGB:
            vertices = (PositionXYZColorRGB*)malloc(numVertices * sizeof(PositionXYZColorRGB));
            // Copy the contents of the other object's environmentTriangleVertices array
            std::copy((PositionXYZColorRGB*)other.vertices, ((PositionXYZColorRGB*)other.vertices) + numVertices, (PositionXYZColorRGB*)vertices);
            break;
    }
}

TriangleStripObject& TriangleStripObject::operator=(TriangleStripObject other) {
    if (this != &other) {
        numVertices = other.numVertices;
        attributeType = other.attributeType;
        switch (attributeType) {
            case POSITION_XYZ:
                vertices = (PositionXYZ*) malloc(numVertices * sizeof(PositionXYZ));
                // Copy the contents of the other object's environmentTriangleVertices array
                std::copy((PositionXYZ*) other.vertices, ((PositionXYZ*) other.vertices) + numVertices, (PositionXYZ*) vertices);
                break;
            case POSITION_XYZ_NORMAL_XYZ:
                vertices = (PositionXYZNormalXYZ*) malloc(numVertices * sizeof(PositionXYZNormalXYZ));
                // Copy the contents of the other object's environmentTriangleVertices array
                std::copy((PositionXYZNormalXYZ*) other.vertices, ((PositionXYZNormalXYZ*) other.vertices) + numVertices, (PositionXYZNormalXYZ*) vertices);
                break;
            case POSITION_XYZ_COLOR_RGB:
                vertices = (PositionXYZColorRGB*) malloc(numVertices * sizeof(PositionXYZColorRGB));
                // Copy the contents of the other object's environmentTriangleVertices array
                std::copy((PositionXYZColorRGB*) other.vertices, ((PositionXYZColorRGB*) other.vertices) + numVertices, (PositionXYZColorRGB*) vertices);
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
