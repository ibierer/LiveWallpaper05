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

    float distanceToOrigin;

    float maxViewAngle;

    float gravity;

    float efficiency;

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

    const string ENVIRONMENT_MAP_VERTEX_SHADER =
            ES_VERSION +
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 inverseViewProjection;\n"
            "out vec3 direction;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = vec4(pos, 1.0);\n"
            "    direction = (inverseViewProjection * vec4(pos, 1.0f)).xyz;\n"
            "}\n";

    const string CUBEMAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision mediump float;\n"
            "uniform samplerCube environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(environmentTexture, direction); \n"
            "}\n";

    const string SPHERE_MAP_FRAGMENT_SHADER =
            ES_VERSION +
            "precision highp float;\n"
            "uniform sampler2D environmentTexture;\n"
            "in vec3 direction;\n"
            "out vec4 outColor;\n" +
            SPHERE_MAP_TEXTURE_FUNCTION +
            "void main() {\n"
            "    outColor = Texture(environmentTexture, direction);\n"
            "}\n";

    static string jstringToString(JNIEnv *env, jstring jStr);

    static string stringArrayToString(string* const strings, const int& count);

    void incrementFrameCount();

    int getFrameCount();

    mat3<float> incrementalRotationMatrix;

protected:

    vec3 computeForce(const float& gravity, const bool& referenceFrameRotates, const Matrix4<float> rotation);

private:

    int framesRendered;

    static const bool supportsES32();

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint buildShaderProgram(const GLenum *const shaderTypes, const char **const sources, const int &shaderCount);

};


#endif //LIVEWALLPAPER05_VIEW_H
