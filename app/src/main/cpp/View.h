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
using std::vector;

class View {
public:

    GLuint mProgram;

    vec4 backgroundColor;

    bool backgroundIsSolidColor;

    static int backgroundTexture;

    EGLContext mEglContext;

    float zoom;

    int orientation;

    EGLint initialWidth = 0;

    EGLint initialHeight = 0;

    EGLint width;

    EGLint height;

    const float zNear = 0.1f;

    const float zFar = 1000.0f;

    vec3 accelerometerVector;

    vec3 linearAccelerationVector;

    vec4 rotationVector;

    Matrix4<float> perspective;

    Matrix4<float> orientationAdjustedPerspective;

    static const PositionXYZ triangleVertices[3];

    static const PositionXYZNormalXYZ triangleVerticesNormals[3];

    static const PositionXYZ tileVertices[4];

    static const PositionXYZ tilesVertices[10];

    View();

    ~View();

    virtual void render();

    static bool checkGlError(const char* const funcName);

    static GLuint createComputeShaderProgram(const char *compSrc);

    static GLuint createVertexAndFragmentShaderProgram(const char* const vtxSrc, const char* const fragSrc);

    static GLuint createVertexGeometryAndFragmentShaderProgram(const char *const vtxSrc, const char *const geomSrc, const char *const fragSrc);

    static GLuint createVertexTesselationControlTesselationEvaluationAndFragmentShaderProgram(const char *const vtxSrc, const char *const tescSrc, const char *const teseSrc, const char *const fragSrc);

    static GLuint createVertexTesselationControlTesselationEvaluationGeometryAndFragmentShaderProgram(const char *const vtxSrc, const char *const tescSrc, const char *const teseSrc, const char *const geomSrc, const char *const fragSrc);

    static void printGlString(const char* const name, const GLenum& s);

    void calculatePerspectiveSetViewport(const float& maxViewDegrees, const float& zNear, const float& zFar);

    vec3 compensateForOrientation(const vec3& acc);

    static const string ES_VERSION;

    static const string SPHERE_MAP_TEXTURE_FUNCTION;

    static const string REFLECT2_FUNCTION;

    static const string REFRACT2_FUNCTION;

    static const string DOUBLE_REFRACT_FUNCTION;

    static const string DOUBLE_REFRACT2_FUNCTION;

    static const string FRESNEL_EFFECT_FUNCTION;

    static string jstringToString(JNIEnv *env, jstring jStr);

    static string stringArrayToString(string* const strings, const int& count);

    void incrementFrameCount();

    int getFrameCount();

protected:

    vec3 computeForce(const float& gravity, const bool& referenceFrameRotates, const Matrix4<float> rotation);

private:

    int framesRendered;

    static const bool supportsES32();

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint buildShaderProgram(const GLenum *const shaderTypes, const char **const sources, const int &shaderCount);

};


#endif //LIVEWALLPAPER05_VIEW_H
