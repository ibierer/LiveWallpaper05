//
// Created by Immanuel Bierer on 12/29/2023.
//

#ifndef LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H
#define LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H


#include "View.h"

class SphereWithReflectionView : public View {
public:

    GLuint _mProgram;

    GLuint mReflectionProgram;

    GLuint mBackgroundProgram;

    VertexArrayObject sphereVAO;

    VertexArrayObject environmentTriangleVAO;

    EnvironmentMap environmentMap;

    const string _REFLECTION_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "uniform mat4 mvp;\n"
            "uniform mat4 viewTransformation;\n"
            "out vec3 direction;\n"
            "out vec3 vNormal;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    direction = (viewTransformation * vec4(pos, 1.0)).xyz;\n"
            "    vNormal = normal;\n"
            "}\n";

    const string _REFLECTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
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
            "    outColor = texture(environmentTexture, directionToSphereMapUV(reflect(direction, vNormal)));\n"
            "}\n";

    const string _BACKGROUND_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string _BACKGROUND_FRAGMENT_SHADER =
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

    const string REFLECTION_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "uniform mat4 mvp;\n"
            "uniform mat4 viewTransformation;\n"
            "out vec3 direction;\n"
            "out vec3 vNormal;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "    direction = (viewTransformation * vec4(pos, 1.0)).xyz;\n"
            "    vNormal = normal;\n"
            "}\n";

    const string REFLECTION_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, reflect(direction, vNormal));\n"
            "}\n";

    const string BACKGROUND_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string BACKGROUND_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, direction); \n"
            "}\n";

    SphereWithReflectionView();

    ~SphereWithReflectionView();

    void render() override;

private:

};


#endif //LIVEWALLPAPER05_SPHEREWITHREFLECTIONVIEW_H
