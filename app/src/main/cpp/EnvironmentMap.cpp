//
// Created by Immanuel Bierer on 12/12/2023.
//

#include "EnvironmentMap.h"

const PositionXYZ EnvironmentMap::environmentTriangleVertices[3] = {
        PositionXYZ(vec3(-1.0f, -1.0f, 0.999f)),
        PositionXYZ(vec3(3.0f, -1.0f, 0.999f)),
        PositionXYZ(vec3(-1.0f, 3.0f, 0.999f))
};
