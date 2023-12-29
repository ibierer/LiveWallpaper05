//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_ENVIRONMENTMAP_H
#define LIVEWALLPAPER05_ENVIRONMENTMAP_H


#include "Texture.h"

class EnvironmentMap : public Texture {
public:

    Vertex environmentTriangleVertices[3] = {
            Vertex(vec3(-1.0f, -1.0f, 0.999f)),
            Vertex(vec3( 3.0f, -1.0f, 0.999f)),
            Vertex(vec3(-1.0f,  3.0f, 0.999f))
    };

private:

};


#endif //LIVEWALLPAPER05_ENVIRONMENTMAP_H
