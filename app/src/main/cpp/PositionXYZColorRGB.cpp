//
// Created by Immanuel Bierer on 12/28/2023.
//

#include "PositionXYZColorRGB.h"

PositionXYZColorRGB::PositionXYZColorRGB() {

}

PositionXYZColorRGB::PositionXYZColorRGB(const vec3& position, const vec3& color) {
    this->p = position;
    this->c = color;
}
