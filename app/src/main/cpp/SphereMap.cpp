//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "SphereMap.h"

SphereMap::SphereMap(Texture::DefaultImages option){
    generateTexture(option);
}

// Destructor
SphereMap::~SphereMap() {

}

// Copy Constructor
SphereMap::SphereMap(const SphereMap& other) {
    textureId = other.textureId;
}

// Assignment Operator
SphereMap& SphereMap::operator=(const SphereMap& other) {
    if (this != &other) {
        textureId = other.textureId;
    }

    return *this;
}