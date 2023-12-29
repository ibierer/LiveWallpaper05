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

    const string VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "vec2 directionToSphereMapUV(vec3 direction) {\n"
            "    // Normalize the direction vector\n"
            "    vec3 normalizedDirection = normalize(direction);\n"
            "    // Calculate polar angle (theta) and azimuthal angle (phi)\n"
            "    float theta = acos(normalizedDirection.z); // polar angle\n"
            "    float phi = atan(normalizedDirection.y, normalizedDirection.x); // azimuthal angle\n"
            "    // Map angles to UV coordinates\n"
            "    float u = phi / (2.0 * 3.14159265359) + 0.5;\n"
            "    float p = 1.0 - theta / 3.14159265359;\n"
            "    return vec2(u, p);\n"
            "}\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, directionToSphereMapUV(direction));\n"
            "}\n";

    SphereMapView();

    ~SphereMapView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREMAPVIEW_H