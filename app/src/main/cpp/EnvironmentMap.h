//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_ENVIRONMENTMAP_H
#define LIVEWALLPAPER05_ENVIRONMENTMAP_H


#include "Texture.h"

class EnvironmentMap : public Texture {
public:

    PositionXYZ environmentTriangleVertices[3] = {
            PositionXYZ(vec3(-1.0f, -1.0f, 0.999f)),
            PositionXYZ(vec3(3.0f, -1.0f, 0.999f)),
            PositionXYZ(vec3(-1.0f, 3.0f, 0.999f))
    };

private:

};


#endif //LIVEWALLPAPER05_ENVIRONMENTMAP_H
