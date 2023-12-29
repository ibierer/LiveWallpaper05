//
// Created by Immanuel Bierer on 12/28/2023.
//

#include "VertexColor.h"

VertexColor::VertexColor() {

}

VertexColor::VertexColor(const vec3& position, const vec3& color) {
    this->v = position;
    this->c = color;
}
