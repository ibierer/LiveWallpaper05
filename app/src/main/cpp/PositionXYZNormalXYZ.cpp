//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "PositionXYZNormalXYZ.h"

PositionXYZNormalXYZ::PositionXYZNormalXYZ() {

}

PositionXYZNormalXYZ::PositionXYZNormalXYZ(const vec3& position, const vec3& normal) {
    this->p = position;
    this->n = normal;
}
