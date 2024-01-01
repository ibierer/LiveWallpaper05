//
// Created by Immanuel Bierer on 12/4/2023.
//

#ifndef LIVEWALLPAPER05_VIEW_H
#define LIVEWALLPAPER05_VIEW_H


#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <string>
#include "PositionXYZ.h"
#include "PositionXYZNormalXYZ.h"
#include "cyCodeBase-master/cyMatrix.h"

using std::string;
using cy::Matrix4;

class View {
public:

    GLuint mProgram;

    vec4 backgroundColor;

    EGLContext mEglContext;

    float zoom;

    int orientation;

    EGLint width;

    EGLint height;

    vec3 accelerometerVector;

    vec4 rotationVector;

    Matrix4<float> perspective;

    Matrix4<float> orientationAdjustedPerspective;

    PositionXYZ triangleVertices[3] = {
            PositionXYZ(vec3(1.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(0.0f, 1.0f, 0.0f)),
            PositionXYZ(vec3(1.0f, 0.0f, 1.0f))
    };

    PositionXYZNormalXYZ triangleVerticesNormals[3] = {
            PositionXYZNormalXYZ(vec3(1.0f, 0.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))),
            PositionXYZNormalXYZ(vec3(0.0f, 1.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))),
            PositionXYZNormalXYZ(vec3(1.0f, 0.0f, 1.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f)))
    };

    PositionXYZ tileVertices[4] = {
            PositionXYZ(vec3(0.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(0.0f, 1.0f, 0.0f)),
            PositionXYZ(vec3(1.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(1.0f, 1.0f, 0.0f))
    };

    PositionXYZ tilesVertices[10] = {
            PositionXYZ(vec3(0.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(0.0f, 1.0f, 0.0f)),
            PositionXYZ(vec3(1.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(1.0f, 1.0f, 0.0f)),
            PositionXYZ(vec3(1.0f, 1.0f, 0.0f)),
            PositionXYZ(vec3(0.0f, 0.0f, 1.0f)),
            PositionXYZ(vec3(0.0f, 0.0f, 1.0f)),
            PositionXYZ(vec3(0.0f, 1.0f, 1.0f)),
            PositionXYZ(vec3(1.0f, 0.0f, 1.0f)),
            PositionXYZ(vec3(1.0f, 1.0f, 1.0f))
    };

    View();

    ~View();

    virtual void render();

    static bool checkGlError(const char* const funcName);

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint createProgram(const char* const vtxSrc, const char* const fragSrc);

    static void printGlString(const char* const name, const GLenum& s);

    void calculatePerspectiveSetViewport(const float& maxViewDegrees);

    const string ES_VERSION = supportsES32() ? "#version 320 es\n" : "#version 310 es\n";

    const string SPHERE_MAP_TEXTURE_FUNCTION =
            "vec4 Texture(sampler2D sampler, vec3 direction) {\n"
                 // Normalize the direction vector
            "    vec3 normalizedDirection = normalize(direction);\n"
                 // Calculate polar angle (theta) and azimuthal angle (phi)
            "    float theta = acos(normalizedDirection.z); // polar angle\n"
            "    float phi = atan(normalizedDirection.y, normalizedDirection.x); // azimuthal angle\n"
                 // Map angles to UV coordinates
            "    float u = phi / (2.0 * 3.14159265359) + 0.5;\n"
            "    float p = 1.0 - theta / 3.14159265359;\n"
            "    return texture(sampler, vec2(u, p));\n"
            "}\n";

    const string REFLECT2_FUNCTION =
            "vec3 reflect2(vec3 I, vec3 N, float dotNI) {\n"
            "    return I - 2.0 * dotNI * N;\n"
            "}\n";

    const string REFRACT2_FUNCTION =
            "vec3 refract2(vec3 I, vec3 N, float eta, float dotNI) {\n"
            "    vec3 R;\n"
            "    float k = 1.0 - eta * eta * (1.0 - dotNI * dotNI);\n"
            "    if (k < 0.0)\n"
            "        return(R = vec3(0.0));\n"
            "    else\n"
            "        R = eta * I - (eta * dotNI + sqrt(k)) * N;\n"
            "    return R;\n"
            "}\n";

    const string DOUBLE_REFRACT_FUNCTION =
            "vec3 doubleRefract(vec3 I, vec3 N, float eta) {\n"
            "    vec3 R;\n"
            "    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));\n"
            "    if (k < 0.0)\n"
            "        return(R = vec3(0.0));\n"
            "    else\n"
            "        R = eta * I - (eta * dot(N, I) + sqrt(k)) * N;\n"
            "    return 2.0f * dot(I, R) * R - I;\n"
            "}\n";

    const string DOUBLE_REFRACT2_FUNCTION =
            "vec3 doubleRefract2(vec3 I, vec3 N, float eta, float dotNI) {\n"
            "    vec3 R;\n"
            "    float k = 1.0 - eta * eta * (1.0 - dotNI * dotNI);\n"
            "    if (k < 0.0)\n"
            "        return(R = vec3(0.0));\n"
            "    else\n"
            "        R = eta * I - (eta * dotNI + sqrt(k)) * N;\n"
            "    return 2.0f * dot(I, R) * R - I;\n"
            "}\n";

    const string FRESNEL_EFFECT_FUNCTION =
            "float fresnel(float dotNI){ \n"
            "    return pow(1.0 + dotNI, 3.0); \n"
            "} \n";

    static string jstringToString(JNIEnv *env, jstring jStr);

    void incrementFrameCount();

    int getFrameCount();

private:

    int framesRendered;

    const static bool supportsES32();

};


#endif //LIVEWALLPAPER05_VIEW_H
