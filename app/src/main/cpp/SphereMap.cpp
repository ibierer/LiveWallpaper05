//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMap.h"

// Default Constructor
SphereMap::SphereMap() {

}

SphereMap::SphereMap(Texture::DefaultImages option, const int &w, const int &h, View *view) {
    generateTexture(option, w, h, view);
}

SphereMap::SphereMap(Texture& texture) {
    textureId = texture.getTextureId();
    width = texture.getWidth();
    height = texture.getHeight();
}

// Destructor
SphereMap::~SphereMap() {

}

// Copy Constructor
SphereMap::SphereMap(const SphereMap& other) {
    textureId = other.textureId;
    width = other.width;
    height = other.height;
}

// Assignment Operator
SphereMap& SphereMap::operator=(const SphereMap& other) {
    if (this != &other) {
        textureId = other.textureId;
        width = other.width;
        height = other.height;
    }

    return *this;
}
