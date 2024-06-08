//
// Created by Immanuel Bierer on 12/12/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREMAPVIEW_H
#define LIVEWALLPAPER05_SPHEREMAPVIEW_H


#include "SphereMap.h"

class SphereMapView : public View {
public:

    VertexArrayObject environmentTriangleVAO;

    SphereMap sphereMap;

    GLuint sphereMapProgram;

    SphereMapView();

    ~SphereMapView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREMAPVIEW_H