//
// Created by Immanuel Bierer on 12/11/2023.
//

#ifndef LIVEWALLPAPER05_CUBEMAPVIEW_H
#define LIVEWALLPAPER05_CUBEMAPVIEW_H


#include "CubeMap.h"

class CubeMapView : public View {
public:

    CubeMap cubeMap;

    VertexArrayObject environmentTriangleVAO;

    CubeMapView();

    ~CubeMapView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_CUBEMAPVIEW_H
