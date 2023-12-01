#include <jni.h>
#include <string>
#include <cstdlib>
#include <string>
#include <ctime>
#include <stack>
#include <android/log.h>
#include <cmath>
#include <EGL/egl.h>
#include "vectors.h"
#include "cyCodeBase-master/cyMatrix.h"

using cy::Matrix4;
using cy::Vec3;
using std::min;
using std::max;

//#define DYNAMIC_ES3 true
#if DYNAMIC_ES3
#include "gl3stub.h"
#else
// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

#endif

#define DEBUG 1

#define LOG_TAG "GLES3JNI"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#if DEBUG
#define ALOGV(...) \
  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define ALOGV(...)
#endif

#define STR(s) #s
#define STRV(s) STR(s)

#define POSITION_ATTRIBUTE_LOCATION 0
#define NORMAL_ATTRIBUTE_LOCATION 1

class Attributes{
public:
    vec3 v;
};

class Vertex : public Attributes{
public:
    Vertex(const vec3& position);
};

Vertex::Vertex(const vec3& position) {
    this->v = position;
}

class VertexNormal : public Attributes {
public:
    vec3 n;
    VertexNormal(const vec3& position, const vec3& normal);
};

VertexNormal::VertexNormal(const vec3& position, const vec3& normal) {
    this->v = position;
    this->n = normal;
}

class Wallpaper{
public:

    int framesRendered;

    EGLContext mEglContext;

    float val;

    EGLint width;

    EGLint height;

    vec3 accelerometerVector;

    vec4 rotationVector;

    Matrix4<float> perspective;

    const Vertex BOX[24] = {
            // FRONT
            {{-0.5f, -0.5f,  0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{-0.5f,  0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            // BACK
            {{-0.5f, -0.5f, -0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            // LEFT
            {{-0.5f, -0.5f,  0.5f}},
            {{-0.5f,  0.5f,  0.5f}},
            {{-0.5f, -0.5f, -0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            // RIGHT
            {{ 0.5f, -0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            // TOP
            {{-0.5f,  0.5f,  0.5f}},
            {{ 0.5f,  0.5f,  0.5f}},
            {{-0.5f,  0.5f, -0.5f}},
            {{ 0.5f,  0.5f, -0.5f}},
            // BOTTOM
            {{-0.5f, -0.5f,  0.5f}},
            {{-0.5f, -0.5f, -0.5f}},
            {{ 0.5f, -0.5f,  0.5f}},
            {{ 0.5f, -0.5f, -0.5f}}
    };

    Wallpaper();

    ~Wallpaper();

    virtual void render();

    static bool checkGlError(const char* const funcName);

    static GLuint createShader(const GLenum& shaderType, const char* const src);

    static GLuint createProgram(const char* const vtxSrc, const char* const fragSrc);

    static void printGlString(const char* const name, const GLenum& s);

    void calculatePerspective();

private:

};

Wallpaper::Wallpaper() : framesRendered(0), val(0), mEglContext(eglGetCurrentContext()){

}

Wallpaper::~Wallpaper(){
    if (eglGetCurrentContext() != mEglContext) return;
}

void Wallpaper::render(){

}

void Wallpaper::calculatePerspective() {
    perspective.SetPerspective(45.0f, (float) width / (float) height, 0.1f, 1000.0f);
}

// returns true if a GL error occurred
bool Wallpaper::checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

GLuint Wallpaper::createShader(const GLenum& shaderType, const char* src) {
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        checkGlError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &src, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                ALOGE("Could not compile %s shader:\n%s\n",
                      shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Wallpaper::createProgram(const char* const vtxSrc, const char* const fragSrc) {
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLuint program = 0;
    GLint linked = GL_FALSE;

    vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader) goto exit;

    fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader) goto exit;

    program = glCreateProgram();
    if (!program) {
        checkGlError("glCreateProgram");
        goto exit;
    }
    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        ALOGE("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                ALOGE("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    exit:
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return program;
}

void Wallpaper::printGlString(const char* const name, const GLenum& s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}

class Box : public Wallpaper{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    Box();

    ~Box();

    void render() override;

private:
};

Box::Box() : Wallpaper(){
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, v));
}

Box::~Box(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(1, mVB);
}

void Box::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, val * 1.0f - 3.0f));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = perspective * translation * rotation;
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glBindVertexArray(mVBState);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glUniform4f(glGetUniformLocation(mProgram, "color"), 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    checkGlError("Renderer::render");
    framesRendered++;
}

class Triangle : public Wallpaper{
public:

    GLuint mProgram;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "out vec3 vNormal;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp*vec4(pos, 1.0);\n"
            "    vNormal = normal;\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
            "}\n";

    Triangle();

    ~Triangle();

    void render() override;

private:
};

Triangle::Triangle() : Wallpaper(){
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
}

Triangle::~Triangle(){
    glDeleteProgram(mProgram);
}

void Triangle::render(){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 10.0f * val - 10.0f));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = perspective * translation * rotation;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    VertexNormal vertices[3] = {
            {vec3(1.0f, 0.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))},
            {vec3(0.0f, 1.0f, 0.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))},
            {vec3(1.0f, 0.0f, 1.0f), vec3(sqrtf(3.0f), sqrt(3.0f), sqrt(3.0f))}
    };
    uvec3 indices[1] = {
            uvec3(0, 1, 2)
    };
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&vertices[0].v);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&vertices[0].n);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
}

#ifndef IMPLICITGRAPHER_H
#define IMPLICITGRAPHER_H

#include <string>
#include "vectors.h"

using std::string;

class ImplicitGrapher {

    static const char pi = 5;

    static const int NAME = 0;

    static const int EQUATION = 1;

    static const int X = -1;

    static const int Y = -2;

    static const int Z = -3;

    static const int T = -4;

    static const int E = -5;

    static const int PI = -6;

    static const int OPEN_PARENTHESIS = -7;

    static const int CLOSE_PARENTHESIS = -8;

    static const int ADD = -9;

    static const int SUBTRACT = -10;

    static const int MULTIPLY = -11;

    static const int DIVIDE = -12;

    static const int POWER = -13;

    static const int SINE = -14;

    static const int COSINE = -15;

    static const int TANGENT = -16;

    static const int ARC_SINE = -17;

    static const int ARC_COSINE = -18;

    static const int ARC_TANGENT = -19;

    static const int HYPERBOLIC_SINE = -20;

    static const int HYPERBOLIC_COSINE = -21;

    static const int HYPERBOLIC_TANGENT = -22;

    static const int HYPERBOLIC_ARC_SINE = -23;

    static const int HYPERBOLIC_ARC_COSINE = -24;

    static const int HYPERBOLIC_ARC_TANGENT = -25;

    static const int ABSOLUTE_VALUE = -26;

    static const int LOG = -27;

    static const int NATURAL_LOG = -28;

    static const int SQUARE_ROOT = -29;

    static const int CUBED_ROOT = -30;

    static const int UNDERSCORE = -31;

    static const int maxEquationLength = 4096;

    static const int maxNumOfEquations = 1024;

    static const int numOfDefaultEquations = 41;

    static const int numOfFunctions = 17;

    string memoryEquations[maxNumOfEquations][2];

    const string defaultEquations[numOfDefaultEquations][2] = {
            {"Two Toroids",                  "1/((sqrt(x^2 + y^2) - 2 + 1.25cos(t))^2 + (z - 1.5sin(t))^2) + 1/((sqrt(x^2 + y^2) - 2 - 1.25cos(t))^2 + (z + 1.5sin(t))^2) = 1.9"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            {"Three Toroids",                "1/((sqrt(x^2 + y^2) - 1.5 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 2/3))^2 + (z + cos(t + 2/3))^2) + 1/((sqrt(x^2 + y^2) - 1.5 + sin(t + 4/3))^2 + (z + cos(t + 4/3))^2) = 5"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                },
            {"Four Toroids",                 "1/((sqrt(x^2 + y^2) - 1.6 + sin(t))^2 + (z + cos(t))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + /2))^2 + (z + cos(t + /2))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + ))^2 + (z + cos(t + ))^2) + 1/((sqrt(x^2 + y^2) - 1.6 + sin(t + 3/2))^2 + (z + cos(t + 3/2))^2) = 7.3"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         },
            {"Expanding Concentric Spheres", "sin(sqrt(x^2 + y^2 + z^2) - t) = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Heart",                        "x^2 + ((y - (x^2)^(1/3.0)))^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            {"Heart II",                     "(0.5x)^2 + (abs(0.5x) - 0.5y)^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Spiral",                       "sqrt((x - sin(z - t))^2 + (y - cos(z - t))^2) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            {"Ripples",                      "3cos(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t + sin(sqrt(x^2 + y^2) - t)/2)/2)/2)/2)/2)/2)/2)/2)/2)/2)/sqrt(x^2 + y^2 + z^2) = z"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    },
            {"Circle",                       "x^2 + y^2 = 4"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Square",                       "-(0.5x)^100 + 1 = 0.5abs(y)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
            {"Sphere",                       "x^2 + y^2 + z^2 = 4"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Cylinder",                     "(0.5x)^2 + (0.5y)^40 + (0.5z)^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Orbit",                        ".2/((x + 2.5sin(t))^2 + (y + 2.5cos(t))^2 + z^2) + 1/((x - 0.5sin(t))^2 + (y - 0.5cos(t))^2 + z^2) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   },
            {"Arch",                         "1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) - z = 1",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           },
            {"Dunking Doughnut",             "3/sqrt((sqrt(x^2 + (z - 6sin(t))^2) - 3)^2 + y^2) - z = 3",                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
            {"Ring around Arch",             "1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) - z + 1/sqrt((sqrt(y^2 + (z - 3)^2) - 1.5)^2 + x^2) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Chain",                        "1/sqrt((sqrt((x + 4.1 + sin(t)/3)^2 + y^2) - 3)^2 + z^2) + 1/sqrt((sqrt(x^2 + z^2) - 3)^2 + y^2) + 1/sqrt((sqrt((x - 4.1 - sin(t)/3)^2 + y^2) - 3)^2 + z^2) = 3"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Smiley Face",                  "(x^4 + y^4 + 2x^2y^2 - 2x^2 - 2y^2 + 1.3*2^y)((x^2 + y^2 - y + .6)^2 - 2x^2)(x^2 + y^2 - 4) = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Pythagorean's Theorem",        "x^2 + y^2 = z^2"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Two Cubes",                    "1/(x^40 + y^40 + z^40) + 1/((x + 4sin(t))^40 + (y + 4cos(t))^40 + z^40) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              },
            {"Hole Thru Cube",               "1/((abs(0.3x) + 0.25)^20 + (abs(0.3y) + 0.25)^20 + (abs(0.3z) + 0.25)^20) - 1/((x)^2 + (y)^2)^100 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    },
            {"Guitar",                       "1/(x^2 + (.75(y + 1.2))^4 + (3z)^40) - .025/((x - .7)^2 + (y + .9)^2) - .025/((x + .7)^2 + (y + .9)^2) - .06/((x - .9)^2 + (y + .85)^2) - .06/((x + .9)^2 + (y + .85)^2) - 1/((4x)^2 + (4(y + .6))^2 + (10(z - .25))^10)^10 - 1/((2x)^2 + (2(y + .6))^2 + (5z)^40)^10 - 1/((1.5x)^2 + (1.4(y + 1.6))^2 + (5z)^40)^10 + 1/((4x)^2 + (y - 1)^40 + (10(z - .255))^2)^10 + 1/((4x/(y - 1))^40 + (3(y - 2.2))^40 + (10(z + .2y - .6))^10)^40 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                              },
            {"Skateboard",                   "1/(80(x + 2)^2 + 80y^2 + z^10)^10 + 1/(80(x - 2)^2 + 80y^2 + z^10)^10 + 1/(8(x + 2)^2 + 8y^2 + 10000000000000000000000000(z - 0.75)^40)^10 + 1/(8(x + 2)^2 + 8y^2 + 10000000000000000000000000(z + 0.75)^40)^10 + 1/(8(x - 2)^2 + 8y^2 + 10000000000000000000000000(z - 0.75)^40)^10 + 1/(8(x - 2)^2 + 8y^2 + 10000000000000000000000000(z + 0.75)^40)^10 + 1/(10(x - 2)^2 + 10(y - 0.25)^2 + 20z^2)^2 + 1/(10(x + 2)^2 + 10(y - 0.25)^2 + 20z^2)^2 + 1/(0.00002x^10 + 10000000000(y - 0.5 - (0.25x)^4)^10 + z^2)^10 = 1"                                                                                                                                                                                                                                                                                                                                                                 },
            {"Miscellaneous 1",              "sqrt(sin(x)^2 + sin(y)^2 + sin(z)^2) - 0.5sin(t) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
            {"Miscellaneous 2",              "sin(x) + sin(y) + sin(z) = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
            {"Miscellaneous 3",              "sin(x^2 + y^2) = cos(xy)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
            {"Miscellaneous 4",              "sqrt(x^2 + y^2) = 1/sqrt((sqrt(x^2 + y^2))^2 + z^2)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      },
            {"Miscellaneous 5",              "abs(y) = 1/sqrt(x^2 + y^2)"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
            {"Miscellaneous 6",              "abs(z) = 1/(x^2 + y^2) - 0.12"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Miscellaneous 7",              "x^3 + 5x^2 + xy^2 - 5y^2 = 0"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             },
            {"Miscellaneous 8",              "xyz = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
            {"Miscellaneous 9",              "x + y + z = xyz"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          },
            {"Miscellaneous 10",             "(x/3)^40 + (y/3)^40 + (z/1.5)^40 + (((x - y)/6)^2 + ((y - x)/6)^2)^20 + (((-x - y)/6)^2 + ((x + y)/6)^2)^20 = 0.99"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Miscellaneous 11",             "sqrt(x^2+y^2) = sin(t) + 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               },
            {"Miscellaneous 12",             "1/((0.5x)^2 + (0.5y)^2 + (0.5z)^2) - 1/(((0.5x) + sin(t))^2 + (0.5y + cos(t))^2 + (0.5z)^2)^40 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Miscellaneous 13",             "1/(x^2 + y^2) + 1/((x - sin(z - t))^2 + (y + cos(z - t))^2)^40 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
            {"Miscellaneous 14",             "x^2 + y^2 = 3atan(y/x)^2"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 },
            {"Miscellaneous 15",             "x^2 + ((y^2)sin(y)) + ((z^2)cos(z)) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  },
            {"Miscellaneous 16",             "(x^2)sin(x)+ ((y^2)cos(y))+ ((z^2)sin(z)) = sin(t)^2"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     },
            {"Miscellaneous 17",             "(x + zsin(z + t))^2 + (y + zcos(z + t))^2 = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            },
            {"Miscellaneous 18",             "1/(x^2 + ((y^2)sin(y)) + z^2) + 0.89/((x - 3sin(t))^2+(y - 3cos(t))^2 + z^40) = 1"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        },
            //{"Flower",                       ""                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       },
    };

    const string functions[numOfFunctions] = {
            "sin(__",
            "cos(__",
            "tan(__",
            "asin(_",
            "acos(_",
            "atan(_",
            "sinh(_",
            "cosh(_",
            "tanh(_",
            "asinh(",
            "acosh(",
            "atanh(",
            "abs(__",
            "log(__",
            "ln(___",
            "sqrt(_",
            "cbrt(_",
    };

    string debug_string;

    double t;

    bool* plusMinus;

    bool* withinGraphRadius;

    ivec3* xyzLineIndex;

    ivec3* groupSegments;

    int constant[maxNumOfEquations][maxEquationLength];

    int valuesCounter[maxNumOfEquations];

    float values[maxEquationLength];

    float equationValues[maxNumOfEquations][maxEquationLength];

    int sequences[maxNumOfEquations][maxEquationLength][3];

    int sequenceLengths[maxNumOfEquations];

    ivec3 radius;

    ivec3 radiusPlusOne;

    ivec3 size;

    ivec3 sizePlus2;

    ivec3 sizePlus3;

    vec3 defaultOffset;

    vec3 currentOffset;

    int maxSolutionCount;

    inline int node0(const int& i, const int& j, const int& k, const int& l);

    inline int node1(const int& i, const int& j, const int& k, const int& l);

    inline int node2(const int& i, const int& j, const int& k, const int& l);

    inline int node3(const int& i, const int& j, const int& k, const int& l);

    int solutionCount;

    int groupSegmentCounter;

    string decode(const int* const codedEquation, const int& length, const float* const values);

    static bool aDigit(const char& character);

    static bool aCharacter(const char& character);

    static string charToString(const char* const characters, const int& length);

    int numOfEquationsInMemory;

    void writeDefaultEquationsToMemory();

    void writeUserEquationsToMemory();

    bool getPlusMinus(const int& i, const int& j, const int& k);

    inline ivec3 getXYZLineIndex(const int& i, const int& j, const int& k);

    void refactor(const ivec3& inputRadius);

    void processEquation(const int& i);

public:

    int surfaceEquation;

    explicit ImplicitGrapher(const ivec3& radius);

    ~ImplicitGrapher();

    VertexNormal* vertices;

    uvec3* indices;

    uint numIndices;

    static float fOfXYZ(ImplicitGrapher& graph, vec3 _, const float& t, const vec3& offset, const float& zoom);

    void calculateSurfaceOnCPU(float (*fOfXYZ)(ImplicitGrapher&, vec3, const float&, const vec3&, const float&), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& vectorPointsPositive, const bool& clipEdges, VertexNormal* _vertices, uvec3* _indices, GLuint& _numIndices);

    // Assignment Constructor
    /*ImplicitGrapher(const ImplicitGrapher& other) {
        // Copy primitive types
        debug_string = other.debug_string;
        t = other.t;
        maxSolutionCount = other.maxSolutionCount;
        surfaceEquation = other.surfaceEquation;
        solutionCount = other.solutionCount;
        groupSegmentCounter = other.groupSegmentCounter;
        numOfEquationsInMemory = other.numOfEquationsInMemory;

        // Copy arrays
        memcpy(plusMinus, other.plusMinus, maxNumOfEquations * sizeof(bool));
        memcpy(withinGraphRadius, other.withinGraphRadius, maxNumOfEquations * sizeof(bool));
        memcpy(xyzLineIndex, other.xyzLineIndex, maxNumOfEquations * sizeof(ivec3));
        memcpy(groupSegments, other.groupSegments, maxNumOfEquations * sizeof(ivec3));

        for (int i = 0; i < maxNumOfEquations; ++i) {
            memcpy(constant[i], other.constant[i], maxEquationLength * sizeof(int));
            valuesCounter[i] = other.valuesCounter[i];
            memcpy(values, other.values, maxEquationLength * sizeof(float));
            memcpy(equationValues[i], other.equationValues[i], maxEquationLength * sizeof(float));
            memcpy(sequences[i], other.sequences[i], maxEquationLength * 3 * sizeof(int));
            sequenceLengths[i] = other.sequenceLengths[i];
        }

        // Copy ivec3 and vec3 types
        radius = other.radius;
        radiusPlusOne = other.radiusPlusOne;
        size = other.size;
        sizePlus2 = other.sizePlus2;
        sizePlus3 = other.sizePlus3;
        defaultOffset = other.defaultOffset;
        currentOffset = other.currentOffset;

        // Copy dynamic arrays (if any) - adjust as needed
        vertices = new VertexNormal[maxSolutionCount];
        memcpy(vertices, other.vertices, maxSolutionCount * sizeof(VertexNormal));

        indices = new uvec3[maxSolutionCount];
        memcpy(indices, other.indices, maxSolutionCount * sizeof(uvec3));
    }

    // Copy Constructor
    ImplicitGrapher& operator=(const ImplicitGrapher& other) {
        if (this != &other) {
            // Release existing resources if any

            // Copy-and-swap idiom for assignment operator
            ImplicitGrapher temp(other);
            std::swap(*this, temp);
        }
        return *this;
    }*/
};

ImplicitGrapher::ImplicitGrapher(const ivec3& radius) {
    refactor(radius);
    plusMinus = (bool*)malloc(sizePlus3.x * sizePlus3.y * sizePlus3.z * sizeof(bool));
    xyzLineIndex = (ivec3*)malloc(sizePlus3.x * sizePlus3.y * sizePlus3.z * sizeof(ivec3));
    groupSegments = (ivec3*)malloc(maxSolutionCount * sizeof(ivec3));
    withinGraphRadius = (bool*)malloc(maxSolutionCount * sizeof(bool));
    vertices = (VertexNormal*)malloc(maxSolutionCount * sizeof(VertexNormal));
    indices = (uvec3*)malloc(3 * maxSolutionCount * sizeof(uvec3));
    writeDefaultEquationsToMemory();
    // writeUserEquationsToMemory();
    for (int i = 0; i < numOfEquationsInMemory; i++) {
        processEquation(i);
    }
}

ImplicitGrapher::~ImplicitGrapher(){
    free(plusMinus);
    free(xyzLineIndex);
    free(groupSegments);
    free(withinGraphRadius);
    free(vertices);
    free(indices);
}

string ImplicitGrapher::decode(const int* const codedEquation, const int& length, const float* const values) {
    string equation = "";
    for (int i = 0; i < length; i++) {
        if (codedEquation[i] < 0) {
            switch (codedEquation[i]) {
                case X:
                    equation += "x";
                    break;
                case Y:
                    equation += "y";
                    break;
                case Z:
                    equation += "z";
                    break;
                case T:
                    equation += "t";
                    break;
                case E:
                    equation += "e";
                    break;
                case PI:
                    equation += pi;
                    break;
                case OPEN_PARENTHESIS:
                    equation += "(";
                    break;
                case CLOSE_PARENTHESIS:
                    equation += ")";
                    break;
                case ADD:
                    equation += "+";
                    break;
                case SUBTRACT:
                    equation += "-";
                    break;
                case MULTIPLY:
                    equation += "*";
                    break;
                case DIVIDE:
                    equation += "/";
                    break;
                case POWER:
                    equation += "^";
                    break;
                case SINE:
                    equation += "sin";
                    break;
                case COSINE:
                    equation += "cos";
                    break;
                case TANGENT:
                    equation += "tan";
                    break;
                case ARC_SINE:
                    equation += "asin";
                    break;
                case ARC_COSINE:
                    equation += "acos";
                    break;
                case ARC_TANGENT:
                    equation += "atan";
                    break;
                case LOG:
                    equation += "log";
                    break;
                case NATURAL_LOG:
                    equation += "ln";
                    break;
                case SQUARE_ROOT:
                    equation += "sqrt";
                    break;
                case CUBED_ROOT:
                    equation += "cbrt";
                    break;
                case UNDERSCORE:
                    equation += "_";
                    break;
            }
        }
        else {
            equation += std::to_string(values[codedEquation[i]]);
        }
    }
    return equation;
}

bool ImplicitGrapher::aDigit(const char& character) {
    return character >= '0' && character <= '9';
}

bool ImplicitGrapher::aCharacter(const char& character) {
    return
            character == 'x' ||
            character == 'y' ||
            character == 'z' ||
            character == 't' ||
            character == 'e' ||
            character == pi;
}

string ImplicitGrapher::charToString(const char* const characters, const int& length) {
    string string;
    for (int i = 0; i < length; i++) {
        string += characters[i];
    }
    return string;
}

void ImplicitGrapher::writeDefaultEquationsToMemory() {
    numOfEquationsInMemory = numOfDefaultEquations;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < numOfDefaultEquations; j++) {
            memoryEquations[j][i].assign(defaultEquations[j][i]);
        }
    }
}

void ImplicitGrapher::writeUserEquationsToMemory() {
    FILE* pFile = fopen("/sdcard/Android/data/com.newequationplotter02/files/user equations.txt", "r");
    if (pFile == NULL)
        return;
    char data[(int)pow(2, 16)];// char * data;
    int dataLengthMinusThree = (int)pow(2, 16) - 3;
    fgets(data, (int)pow(2, 16), pFile);
    for (int i = 0; !(data[i] == 'e' && data[i + 1] == 'o' && data[i + 2] == 'f') && i < dataLengthMinusThree; numOfEquationsInMemory++) {
        for (int j = 0; j < 2 && i < dataLengthMinusThree; j++) {
            while (data[i] != '~' && i < dataLengthMinusThree) {
                memoryEquations[numOfEquationsInMemory][j] += data[i];
                i++;
            }
            i++;
        }
    }
    fclose(pFile);
}

void ImplicitGrapher::processEquation(const int& i) {
    bool debugEquation = false;
    for (int j = 0; j < maxEquationLength; j++) {
        constant[i][j] = 0;
    }

    char equation[2 * maxEquationLength];
    int length = memoryEquations[i][EQUATION].length();

    // Convert string to char array
    for (int j = 0; j < length; j++) {
        equation[j] = memoryEquations[i][EQUATION].at(j);
    }
    // Remove spaces
    // Remove spaces and fill remainder with underscores");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string = "Remove spaces and fill remainder with underscores:";
    }
    valuesCounter[i] = 0;
    for (int j = 0; j < length; valuesCounter[i]++) {
        if (equation[valuesCounter[i]] != ' ') {
            equation[j] = equation[valuesCounter[i]];
            j++;
        }
        else {
            length--;
        }
    }

    // Fill remainder with underscores
    for (int j = length; j < 2 * maxEquationLength; j++) {
        equation[j] = '_';
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Isolate and convert '=' to '-'
    // Isolate and convert '=' to '-'");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Isolate and convert '=' to '-':";
    }
    equation[length + 3] = ')';
    valuesCounter[i] = length - 1;
    while (equation[valuesCounter[i]] != '=') {
        equation[valuesCounter[i] + 3] = equation[valuesCounter[i]];
        valuesCounter[i]--;
    }
    equation[valuesCounter[i] + 3] = '(';
    equation[valuesCounter[i] + 2] = '-';
    equation[valuesCounter[i] + 1] = ')';
    valuesCounter[i]--;
    while (valuesCounter[i] > -1) {
        equation[valuesCounter[i] + 1] = equation[valuesCounter[i]];
        valuesCounter[i]--;
    }
    equation[0] = '(';
    length += 4;
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add parentheses to ends
    // Add parentheses to ends");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add parentheses to ends:";
    }
    //Insert close parenthesis
    equation[length + 1] = ')';
    //Move characters
    for (int j = length; j > 0; j--) {
        equation[j] = equation[j - 1];
    }
    length += 2;
    //Insert open parenthesis
    equation[0] = '(';
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add parentheses to functions
    // Add parentheses to functions:");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add parentheses to functions:";
    }
    for (int j = 0; j < length - 1; j++) {//index equation
        bool functionFound = false;
        for (int k = 0; k < numOfFunctions && !functionFound; k++) {//index functions
            for (int l = j, functionLength = 0; l < length && !functionFound && equation[l] == functions[k][functionLength]; l++, functionLength++) {//index function characters
                if (equation[l] == '(') {
                    functionFound = true;
                    l++;
                    for (int parentheses = 1; parentheses > 0; l++) {
                        if (equation[l] == ')') {
                            parentheses--;
                        }
                        else if (equation[l] == '(') {
                            parentheses++;
                        }
                    }
                    if (!(equation[j - 1] == '(' && equation[l] == ')')) {
                        length += 2;
                        //Move characters
                        for (int m = length - 1; m > l + 1; m--) {
                            equation[m] = equation[m - 2];
                        }
                        //Insert close parenthesis
                        equation[l + 1] = ')';
                        //Move characters
                        for (int m = l; m > j; m--) {
                            equation[m] = equation[m - 1];
                        }
                        //Insert open parenthesis
                        equation[j] = '(';
                    }
                    j += functionLength;
                }
            }
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add multiplication signs to close terms
    // Add multiplication signs to close terms:");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add multiplication signs to close terms:";
    }
    for (int j = 0; j < length; j++) {
        bool notR = true;
        if (j > 0) {
            if (equation[j - 1] == 'r') {
                notR = false;
            }
        }
        if (
                (
                        equation[j] == ')' ||
                        aDigit(equation[j]) ||
                        (
                                aCharacter(equation[j]) &&
                                (
                                        equation[j] != 't' || notR
                                )
                        )
                ) && (
                        aCharacter(equation[j + 1]) ||
                        equation[j + 1] == '(' ||
                        aDigit(equation[j + 1])
                ) && (
                        !aDigit(equation[j]) ||
                        !aDigit(equation[j + 1])
                )
                ) {
            for (int k = length; k > j; k--) {
                equation[k] = equation[k - 1];
            }
            equation[j + 1] = '*';
            length++;
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Convert (-x to (0-x or (-1 to (0-1
    // Convert (-x to ((0-x) or (-1 to ((0-1)");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Convert (-x to ((0-x) or (-1 to ((0-1):";
    }
    for (int j = 0; j < length; j++) {
        if (equation[j] == '(' && equation[j + 1] == '-') {
            //Move characters
            length++;
            for (int k = length - 1; k > j + 1; k--) {
                equation[k] = equation[k - 1];
            }
            //Insert zero
            equation[j + 1] = '0';
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length);
    }

    //Add parentheses to symbol functions
    // Add parentheses to symbol functions");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Add parentheses to symbol functions:";
    }
    for (int sweep = 1; sweep < 4; sweep++) {
        int whatJEquals, increment;
        if (sweep == 1) {
            whatJEquals = length - 1;
            increment = -1;
        }
        else {
            whatJEquals = 1;
            increment = 1;
        }
        for (int j = whatJEquals; (sweep == 1 && j > 0) || (sweep > 1 && j < length); j += increment) {//Index equation
            if (
                    (
                            sweep == 1 && equation[j] == '^'
                    ) || (
                            sweep == 2 && (equation[j] == '*' || equation[j] == '/')
                    ) || (
                            sweep == 3 && (equation[j] == '+' || equation[j] == '-')
                    )
                    ) {
                int mode = 0;
                const int DIGIT = 1;
                const int LETTER = 2;
                const int PARENTHESIS = 3;
                int positions[2] = { 0, 0 };
                for (int k = 1, position = 0; k > -2; k -= 2, position++) {
                    if (aDigit(equation[j + k]) || equation[j + k] == '.') {
                        mode = DIGIT;
                    }
                    else if (aCharacter(equation[j + k])) {
                        mode = LETTER;
                    }
                    else if (
                            (equation[j + k] == '(' && k == 1) ||
                            (equation[j + k] == ')' && k == -1)
                            ) {
                        mode = PARENTHESIS;
                    }
                    positions[position] = j + 2 - 4 * position;
                    switch (mode) {
                        case DIGIT:
                            while (
                                    aDigit(equation[positions[position]]) ||
                                    equation[positions[position]] == '.'
                                    ) {
                                positions[position] += k;
                            }
                            break;
                        case LETTER:
                            break;
                        case PARENTHESIS:
                            for (int parentheses = 1; parentheses > 0; positions[position] += k) {
                                if (equation[positions[position]] == ')') {
                                    parentheses -= k;
                                }
                                else if (equation[positions[position]] == '(') {
                                    parentheses += k;
                                }
                            }
                            break;
                    }
                }
                if (!(equation[positions[1]] == '(' && equation[positions[0]] == ')')) {
                    length += 2;
                    //Move characters
                    for (int k = length - 1; k > positions[0] + 1; k--) {
                        equation[k] = equation[k - 2];
                    }
                    //Insert close parenthesis
                    equation[positions[0] + 1] = ')';
                    //Move characters
                    for (int k = positions[0]; k > positions[1]; k--) {
                        equation[k] = equation[k - 1];
                    }
                    //Insert close parenthesis
                    equation[positions[1] + 1] = '(';
                }
            }
        }
    }
    // equation = " + charToString(equation));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += charToString(equation, length) + ", ";
    }

    //Convert to coded equation
    // Convert to coded equation");
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += "Convert to coded equation:";
    }
    int codedEquation[2 * maxEquationLength];
    int codedEquationCounter = 0;
    valuesCounter[i] = 0;
    for (int j = 0; j < length; j++, codedEquationCounter++) {
        if (equation[j] != '.' && !aDigit(equation[j])) {
            if (equation[j] == 'x') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = X;
                valuesCounter[i]++;
            }
            else if (equation[j] == 'y') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = Y;
                valuesCounter[i]++;
            }
            else if (equation[j] == 'z') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = Z;
                valuesCounter[i]++;
            }
            else if (equation[j] == 't' && equation[j + 1] != 'a') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = T;
                valuesCounter[i]++;
            }
            else if (equation[j] == 'e') {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = E;
                valuesCounter[i]++;
            }
            else if (equation[j] == pi) {
                codedEquation[codedEquationCounter] = constant[i][valuesCounter[i]] = PI;
                valuesCounter[i]++;
            }
            else if (equation[j] == '(') {
                codedEquation[codedEquationCounter] = OPEN_PARENTHESIS;
            }
            else if (equation[j] == ')') {
                codedEquation[codedEquationCounter] = CLOSE_PARENTHESIS;
            }
            else if (equation[j] == '+') {
                codedEquation[codedEquationCounter] = ADD;
            }
            else if (equation[j] == '-') {
                codedEquation[codedEquationCounter] = SUBTRACT;
            }
            else if (equation[j] == '*') {
                codedEquation[codedEquationCounter] = MULTIPLY;
            }
            else if (equation[j] == '/') {
                codedEquation[codedEquationCounter] = DIVIDE;
            }
            else if (equation[j] == '^') {
                codedEquation[codedEquationCounter] = POWER;
            }
            else if (equation[j] == functions[0][0] && equation[j + 1] == functions[0][1]) {
                if (equation[j + 3] == functions[0][3]) {
                    codedEquation[codedEquationCounter] = SINE;
                    j += 2;
                }
                else if (equation[j + 3] == functions[6][3]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_SINE;
                    j += 3;
                }
            }
            else if (equation[j] == functions[1][0] && equation[j + 1] == functions[1][1]) {
                if (equation[j + 3] == functions[1][3]) {
                    codedEquation[codedEquationCounter] = COSINE;
                    j += 2;
                }
                else if (equation[j + 3] == functions[7][3]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_COSINE;
                    j += 3;
                }
            }
            else if (equation[j] == functions[2][0] && equation[j + 1] == functions[2][1]) {
                if (equation[j + 3] == functions[2][3]) {
                    codedEquation[codedEquationCounter] = TANGENT;
                    j += 2;
                }
                else if (equation[j + 3] == functions[8][3]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_TANGENT;
                    j += 3;
                }
            }
            else if (equation[j] == functions[3][0] && equation[j + 1] == functions[3][1]) {
                if (equation[j + 4] == functions[3][4]) {
                    codedEquation[codedEquationCounter] = ARC_SINE;
                    j += 3;
                }
                else if (equation[j + 4] == functions[9][4]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_ARC_SINE;
                    j += 4;
                }
            }
            else if (equation[j] == functions[4][0] && equation[j + 1] == functions[4][1]) {
                if (equation[j + 4] == functions[4][4]) {
                    codedEquation[codedEquationCounter] = ARC_COSINE;
                    j += 3;
                }
                else if (equation[j + 4] == functions[10][4]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_ARC_COSINE;
                    j += 4;
                }
            }
            else if (equation[j] == functions[5][0] && equation[j + 1] == functions[5][1]) {
                if (equation[j + 4] == functions[5][4]) {
                    codedEquation[codedEquationCounter] = ARC_TANGENT;
                    j += 3;
                }
                else if (equation[j + 4] == functions[11][4]) {
                    codedEquation[codedEquationCounter] = HYPERBOLIC_ARC_TANGENT;
                    j += 4;
                }
            }
            else if (equation[j] == functions[12][0] && equation[j + 1] == functions[12][1]) {
                codedEquation[codedEquationCounter] = ABSOLUTE_VALUE;
                j += 2;
            }
            else if (equation[j] == functions[13][0] && equation[j + 1] == functions[13][1]) {
                codedEquation[codedEquationCounter] = LOG;
                j += 2;
            }
            else if (equation[j] == functions[14][0] && equation[j + 1] == functions[14][1]) {
                codedEquation[codedEquationCounter] = NATURAL_LOG;
                j += 1;
            }
            else if (equation[j] == functions[15][0] && equation[j + 1] == functions[15][1]) {
                codedEquation[codedEquationCounter] = SQUARE_ROOT;
                j += 3;
            }
            else if (equation[j] == functions[16][0] && equation[j + 1] == functions[16][1]) {
                codedEquation[codedEquationCounter] = CUBED_ROOT;
                j += 3;
            }
            else if (equation[j] == '_') {
                codedEquation[codedEquationCounter] = UNDERSCORE;
            }
        }
        else {
            int numberSize = 0;
            for (int k = j; equation[k] == '.' || aDigit(equation[k]); k++) {
                numberSize++;
            }
            float number = 0;
            int power = 0;
            for (int jMinusOne = j - 1, k = jMinusOne + numberSize; k > jMinusOne; k--, power++) {
                if (equation[k] == '.') {
                    number *= pow(10, -power);
                    power = -1;
                }
                else if (aDigit(equation[k])) {
                    number += (equation[k] - 48) * pow(10, power);
                }
            }
            codedEquation[codedEquationCounter] = valuesCounter[i];
            equationValues[i][valuesCounter[i]] = values[valuesCounter[i]] = number;
            valuesCounter[i]++;
            j += numberSize - 1;
        }
    }
    //Display equation
    // original equation = " + PlotterSettings.defaultEquations[i][EQUATION]);
    // array = " + charToString(equation));
    // coded equation = " + decode(codedEquation, values));
    if (i == numOfEquationsInMemory - 1 && debugEquation) {
        debug_string += decode(codedEquation, codedEquationCounter, values);
    }

    //Convert 0-2 to -2
    //Convert 2-0 to 2
    //Convert 0+2 to 2
    //Convert 2+0 to 2
    //Convert 0*2 to 0
    //Convert 2*0 to 0
    //Convert 0/2 to 0
    //Convert 2/0 to error

    //Create Sequence Table destroying coded equation
    // Create Sequence Table destroying coded equation:");
    for (int j = 0, counter = 0; j < codedEquationCounter; j++) {
        if (codedEquation[j] > -1) {
            counter++;
        }
        else if (codedEquation[j] > -7) {
            codedEquation[j] = counter;
            counter++;
        }
    }
    // coded equation = " + decode(codedEquation, values));
    bool end = false;
    for (sequenceLengths[i] = 0; !end; ) {
        for (int j = 0, openParenthesis = 0; j < codedEquationCounter; j++) {
            if (j > codedEquationCounter - 2) {
                end = true;
            }
            switch (codedEquation[j]) {
                case OPEN_PARENTHESIS:
                    openParenthesis = j;
                    break;
                case CLOSE_PARENTHESIS:
                    switch (j - openParenthesis) {
                        case 1:// "()"
                            for (int k = j - 1; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 2];
                            }
                        case 2:// "(a)"
                            codedEquation[j - 2] = codedEquation[j - 1];
                            for (int k = j - 1; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 2];
                            }
                            break;
                        case 3: {// "(fa)"
                            sequences[i][sequenceLengths[i]][0] = codedEquation[j - 2];//function
                            sequences[i][sequenceLengths[i]][1] = codedEquation[j - 1];//where first value is and where result will be stored
                            codedEquation[j - 3] = codedEquation[j - 1];
                            for (int k = j - 2; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 3];
                            }
                            sequenceLengths[i]++; }
                            break;
                        case 4: {// "(a+b)" "(a-b)" "(a*b)" "(a/b)" "(a^b)"
                            sequences[i][sequenceLengths[i]][0] = codedEquation[j - 2];//function
                            sequences[i][sequenceLengths[i]][1] = codedEquation[j - 3];//where first value is and where result will be stored
                            sequences[i][sequenceLengths[i]][2] = codedEquation[j - 1];//where second value is
                            codedEquation[j - 4] = codedEquation[j - 3];
                            for (int k = j - 3; k < codedEquationCounter; k++) {
                                codedEquation[k] = codedEquation[k + 4];
                            }
                            sequenceLengths[i]++; }
                            break;
                    }
                    // coded equation = " + decode(codedEquation, values));
                    j = codedEquationCounter - 1;
                    break;
            }
        }
    }
}

inline int ImplicitGrapher::node0(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i - 1, j, k - 1)[0]; break;//XY-plane[Yz]
        case 1: return getXYZLineIndex(i - 1, j, k)[0]; break;//XY-plane[YZ]
        case 2: return getXYZLineIndex(i - 1, j - 1, k)[1]; break;//YZ-plane[xZ]
        case 3: return getXYZLineIndex(i, j - 1, k)[1]; break;//YZ-plane[XZ]
        case 4: return getXYZLineIndex(i, j - 1, k - 1)[2]; break;//ZX-plane[Xy]
        case 5: return getXYZLineIndex(i, j, k - 1)[2]; break;//ZX-plane[XY]
        default: return 0;
    }
}

inline int ImplicitGrapher::node1(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i - 1, j - 1, k - 1)[1]; break;//XY-plane[xz]
        case 1: return getXYZLineIndex(i - 1, j - 1, k)[1]; break;//XY-plane[xZ]
        case 2: return getXYZLineIndex(i - 1, j - 1, k - 1)[2]; break;//YZ-plane[xy]
        case 3: return getXYZLineIndex(i, j - 1, k - 1)[2]; break;//YZ-plane[Xy]
        case 4: return getXYZLineIndex(i - 1, j - 1, k - 1)[0]; break;//ZX-plane[yz]
        case 5: return getXYZLineIndex(i - 1, j, k - 1)[0]; break;//ZX-plane[Yz]
        default: return 0;
    }
}

inline int ImplicitGrapher::node2(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i, j - 1, k - 1)[1]; break;//XY-plane[Xz]
        case 1: return getXYZLineIndex(i, j - 1, k)[1]; break;//XY-plane[XZ]
        case 2: return getXYZLineIndex(i - 1, j, k - 1)[2]; break;//YZ-plane[xY]
        case 3: return getXYZLineIndex(i, j, k - 1)[2]; break;//YZ-plane[XY]
        case 4: return getXYZLineIndex(i - 1, j - 1, k)[0]; break;//ZX-plane[yZ]
        case 5: return getXYZLineIndex(i - 1, j, k)[0]; break;//ZX-plane[YZ]
        default: return 0;
    }
}

inline int ImplicitGrapher::node3(const int& i, const int& j, const int& k, const int& l) {
    switch (l) {
        case 0: return getXYZLineIndex(i - 1, j - 1, k - 1)[0]; break;//XY-plane[yz]
        case 1: return getXYZLineIndex(i - 1, j - 1, k)[0]; break;//XY-plane[yZ]
        case 2: return getXYZLineIndex(i - 1, j - 1, k - 1)[1]; break;//YZ-plane[xz]
        case 3: return getXYZLineIndex(i, j - 1, k - 1)[1]; break;//YZ-plane[Xz]
        case 4: return getXYZLineIndex(i - 1, j - 1, k - 1)[2]; break;//ZX-plane[xy]
        case 5: return getXYZLineIndex(i - 1, j, k - 1)[2]; break;//ZX-plane[xY]
        default: return 0;
    }
}

// Calculates the difference between f(x, y, z) and 0.
float ImplicitGrapher::fOfXYZ(ImplicitGrapher &graph, vec3 _, const float &t, const vec3 &offset, const float &zoom) {
    _ -= offset;
    _ *= zoom;
    for (int i = 0; i < graph.valuesCounter[graph.surfaceEquation]; i++) {
        switch (graph.constant[graph.surfaceEquation][i]) {
            case 0: graph.values[i] = graph.equationValues[graph.surfaceEquation][i]; break;
            case X: graph.values[i] = _.x; break;
            case Y: graph.values[i] = _.y; break;
            case Z: graph.values[i] = _.z; break;
            case T: graph.values[i] = t; break;
            case E: graph.values[i] = M_E; break;
            case PI: graph.values[i] = M_PI; break;
        }
    }
    // A better solution to below may be near the bottom of the following web page: https://www.cplusplus.com/doc/tutorial/pointers/
    int e = graph.surfaceEquation;
    float* v = graph.values;
    for (int i = 0; i < graph.sequenceLengths[e]; i++) {
        int* s = graph.sequences[e][i];
        switch (s[0]) {
            case ADD: v[s[1]] += v[s[2]]; break;
            case SUBTRACT: v[s[1]] -= v[s[2]]; break;
            case MULTIPLY: v[s[1]] *= v[s[2]]; break;
            case DIVIDE: v[s[1]] /= v[s[2]]; break;
            case POWER: v[s[1]] = powf(v[s[1]], v[s[2]]); break;
            case SINE: v[s[1]] = sinf(v[s[1]]); break;
            case COSINE: v[s[1]] = cosf(v[s[1]]); break;
            case TANGENT: v[s[1]] = tanf(v[s[1]]); break;
            case ARC_SINE: v[s[1]] = asinf(v[s[1]]); break;
            case ARC_COSINE: v[s[1]] = acosf(v[s[1]]); break;
            case ARC_TANGENT: v[s[1]] = atanf(v[s[1]]); break;
            case HYPERBOLIC_SINE: v[s[1]] = sinhf(v[s[1]]); break;
            case HYPERBOLIC_COSINE: v[s[1]] = coshf(v[s[1]]); break;
            case HYPERBOLIC_TANGENT: v[s[1]] = tanhf(v[s[1]]); break;
            case HYPERBOLIC_ARC_SINE: v[s[1]] = asinhf(v[s[1]]); break;
            case HYPERBOLIC_ARC_COSINE: v[s[1]] = acoshf(v[s[1]]); break;
            case HYPERBOLIC_ARC_TANGENT: v[s[1]] = atanhf(v[s[1]]); break;
            case ABSOLUTE_VALUE: v[s[1]] = abs(v[s[1]]); break;
            case LOG: v[s[1]] = log10f(v[s[1]]); break;
            case NATURAL_LOG: v[s[1]] = logf(v[s[1]]); break;
            case SQUARE_ROOT: v[s[1]] = sqrtf(v[s[1]]); break;
            case CUBED_ROOT: v[s[1]] = cbrtf(v[s[1]]); break;
        }
    }
    return v[0];
}

void ImplicitGrapher::calculateSurfaceOnCPU(float (*fOfXYZ)(ImplicitGrapher& graph, vec3, const float&, const vec3&, const float&), const float& timeVariable, const uint& iterations, const vec3& offset, const float& zoom, const bool& vectorPointsPositive, const bool& clipEdges, VertexNormal* _vertices, uvec3* _indices, GLuint& _numIndices) {
    t = timeVariable;
    currentOffset = defaultOffset + offset;
    // Erase normals
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].n = vec3(0.0f);
    }
    // Reset solution count
    solutionCount = 0;
    // Reset group segment counter
    groupSegmentCounter = 0;
    // Calculate plusMinusAmounts and approximate solutions
    for (int i = 0; i < sizePlus3.x; i++) {
        for (int j = 0; j < sizePlus3.y; j++) {
            for (int k = 0; k < sizePlus3.z; k++) {
                plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k] = fOfXYZ(*this, vec3(i, j, k), t, currentOffset, zoom) > 0.0f;
                uvec3 ijk(i, j, k);
                for (int l = 0; l < 3; l++) {
                    if (ijk[l] == 0) {
                        continue;
                    }
                    uvec3 iCursor = ijk;
                    iCursor[l] -= 1;
                    bool firstSample = plusMinus[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z];
                    bool secondSample = plusMinus[(ijk.x * sizePlus3.y + ijk.y) * sizePlus3.z + ijk.z];
                    // If no solution is detected jump to next iteration of loop.
                    if (!(firstSample ^ secondSample)) {
                        continue;
                    }
                    xyzLineIndex[(iCursor.x * sizePlus3.y + iCursor.y) * sizePlus3.z + iCursor.z][l] = solutionCount;
                    const int POSITIVE = 1;
                    const int NEGATIVE = -1;
                    int sign = secondSample ? POSITIVE : NEGATIVE;
                    for (int m = 0; m < 3; m++) {
                        _vertices[solutionCount].v[m] = iCursor[m];
                    }
                    float scan = 0.5f;
                    for (int m = 0; m < iterations; m++) {// Maybe use a do-while loop here to reduce the scan operations by 1.
                        _vertices[solutionCount].v[l] += scan;
                        if (sign == POSITIVE ^ fOfXYZ(*this, _vertices[solutionCount].v, t, currentOffset, zoom) > 0.0f) {
                            scan *= 0.5f;
                        }else{
                            sign *= -1;
                            scan *= -0.5f;
                        }
                    }
                    solutionCount++;
                }
                if (i == 0 || j == 0 || k == 0) {
                    continue;
                }
                int groupSegmentStartIndex = groupSegmentCounter;
                bool xyz = getPlusMinus(i - 1, j - 1, k - 1);
                bool xyZ = getPlusMinus(i - 1, j - 1, k);
                bool xYz = getPlusMinus(i - 1, j, k - 1);
                bool xYZ = getPlusMinus(i - 1, j, k);
                bool Xyz = getPlusMinus(i, j - 1, k - 1);
                bool XyZ = getPlusMinus(i, j - 1, k);
                bool XYz = getPlusMinus(i, j, k - 1);
                bool XYZ = getPlusMinus(i, j, k);
                for (int l = 0; l < 6; l++) {
                    int combo = 0;
                    switch (l) {
                        case 0://XY
                            combo += xYz * 8;
                            combo += XYz * 4;
                            combo += xyz * 2;
                            combo += Xyz * 1;
                            break;
                        case 1://XY
                            combo += xYZ * 8;
                            combo += XYZ * 4;
                            combo += xyZ * 2;
                            combo += XyZ * 1;
                            break;
                        case 2://YZ
                            combo += xyZ * 8;
                            combo += xYZ * 4;
                            combo += xyz * 2;
                            combo += xYz * 1;
                            break;
                        case 3://YZ
                            combo += XyZ * 8;
                            combo += XYZ * 4;
                            combo += Xyz * 2;
                            combo += XYz * 1;
                            break;
                        case 4://ZX
                            combo += Xyz * 8;
                            combo += XyZ * 4;
                            combo += xyz * 2;
                            combo += xyZ * 1;
                            break;
                        case 5://ZX
                            combo += XYz * 8;
                            combo += XYZ * 4;
                            combo += xYz * 2;
                            combo += xYZ * 1;
                            break;
                    }
                    switch (combo) {
                        case 0: break;//0000
                        case 1:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            break;//0001
                        case 2:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            break;//0010
                        case 3:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1); break;//0011
                        case 4:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            break;//0100
                        case 5:
                            if(l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            break;//0101
                        case 6: {//0110
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ(*this, (_vertices[_node0].v + _vertices[_node1].v + _vertices[_node2].v + _vertices[_node3].v) * 0.25f, t, currentOffset, zoom) > 0.0f) {
                                if (l % 2 != vectorPointsPositive){
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);
                                }
                            } else {
                                if (l % 2 != vectorPointsPositive) {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);
                                }
                            }
                            break;
                        }
                        case 7:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            break;//0111
                        case 8:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node0(i, j, k, l), -1);
                            break;//1000
                        case 9: {//1001
                            int _node0 = node0(i, j, k, l);
                            int _node1 = node1(i, j, k, l);
                            int _node2 = node2(i, j, k, l);
                            int _node3 = node3(i, j, k, l);
                            if (fOfXYZ(*this, (_vertices[_node0].v + _vertices[_node1].v + _vertices[_node2].v + _vertices[_node3].v) * 0.25f, t, currentOffset, zoom) > 0.0f) {
                                if (l % 2 != vectorPointsPositive) {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node2, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node1, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node0, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node3, -1);
                                }
                            } else {
                                if (l % 2 != vectorPointsPositive) {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node3, _node2, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node0, _node1, -1);
                                } else {
                                    groupSegments[groupSegmentCounter++] = ivec3(_node2, _node3, -1);
                                    groupSegments[groupSegmentCounter++] = ivec3(_node1, _node0, -1);
                                }
                            }
                            break;
                        }
                        case 10:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node0(i, j, k, l), -1); break;//1010
                        case 11:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node0(i, j, k, l), node2(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node0(i, j, k, l), -1); break;//1011
                        case 12:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node2(i, j, k, l), -1); break;//1100
                        case 13:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node1(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node1(i, j, k, l), node3(i, j, k, l), -1); break;//1101
                        case 14:
                            if (l % 2 != vectorPointsPositive)
                                groupSegments[groupSegmentCounter++] = ivec3(node2(i, j, k, l), node3(i, j, k, l), -1);
                            else
                                groupSegments[groupSegmentCounter++] = ivec3(node3(i, j, k, l), node2(i, j, k, l), -1); break;//1110
                        case 15:
                            break;//1111
                    }
                }
                //Grouping
                //{73}{64}{0}    {73}{64}{0}    {73}{64}{107}
                //{97}{21}{0}    {64}{89}{0}    {64}{89}{107}
                //{89}{64}{0}    {89}{80}{0}    {89}{80}{107}
                //{89}{80}{0}    {80}{73}{0}    {80}{73}{107}
                //{54}{21}{0}    {21}{97}{0}    {21}{97}{54}
                //{54}{97}{0}    {97}{54}{0}    {97}{54}{0}
                //{80}{73}{0}    {54}{21}{0}    {54}{21}{0}
                int segPerGroup = 1;
                if (clipEdges) {
                    for (int l = groupSegmentStartIndex; l < groupSegmentCounter; l++) {
                        withinGraphRadius[l] = i > 1 && j > 1 && k > 1 && i < sizePlus2.x && j < sizePlus2.y && k < sizePlus2.z;
                    }
                }
                for (int l = groupSegmentStartIndex; l < groupSegmentCounter - 1; l++) {
                    for (int _m = l + 1; _m < groupSegmentCounter; _m++) {
                        if (groupSegments[_m][0] != groupSegments[l][1]) {
                            continue;
                        }
                        //Swap
                        int storeA = groupSegments[l + 1][0];
                        int storeB = groupSegments[l + 1][1];
                        groupSegments[l + 1][0] = groupSegments[_m][0];
                        groupSegments[l + 1][1] = groupSegments[_m][1];
                        groupSegments[_m][0] = storeA;
                        groupSegments[_m][1] = storeB;
                        segPerGroup++;
                        //Check to see if end of loop
                        if (groupSegments[l + 1][1] != groupSegments[l + 2 - segPerGroup][0]) {
                            continue;
                        }
                        if (segPerGroup == 3) {
                            groupSegments[l - 1][2] = groupSegments[l][1];
                        }
                        else {
                            vec3 sum(0.0f);
                            for (int _n = l - segPerGroup + 2; _n < l + 2; _n++) {
                                sum += _vertices[groupSegments[_n][0]].v;
                                groupSegments[_n][2] = solutionCount;
                            }
                            _vertices[solutionCount].v = sum / segPerGroup;
                            solutionCount++;
                        }
                        segPerGroup = 1;
                        l++;
                        break;
                    }
                }
            }
        }
    }

    // Calculate face normals and add them to the per-solution normals.
    for (int i = 0; i < groupSegmentCounter; i++) {
        if (groupSegments[i][2] > -1) {
            vec3 vectorA = _vertices[groupSegments[i][0]].v - _vertices[groupSegments[i][2]].v;
            vec3 vectorB = _vertices[groupSegments[i][1]].v - _vertices[groupSegments[i][2]].v;
            vec3 crossProduct = cross(vectorA, vectorB);
            float length = distance(crossProduct);
            vec3 normalizedCrossProduct = crossProduct / length;
            _vertices[groupSegments[i][0]].n += normalizedCrossProduct;
            _vertices[groupSegments[i][1]].n += normalizedCrossProduct;
            _vertices[groupSegments[i][2]].n += normalizedCrossProduct;
        }
    }
    //Normalize
    for (int i = 0; i < solutionCount; i++) {
        float length = (vectorPointsPositive) ? -distance(vec3(_vertices[i].n)) : distance(vec3(_vertices[i].n));
        _vertices[i].n /= length;
    }
    //Create Triangle Primitives
    _numIndices = 0;
    int triangleCount = 0;
    for (int i = 0; i < groupSegmentCounter; i++) {
        if (groupSegments[i][2] > -1 && (!clipEdges || withinGraphRadius[i])) {
            _indices[triangleCount++] = groupSegments[i];
            _numIndices += 3;
        }
    }
    for (int i = 0; i < solutionCount; i++) {
        _vertices[i].v -= currentOffset;
    }
}

bool ImplicitGrapher::getPlusMinus(const int& i, const int& j, const int& k){
    return plusMinus[(i * sizePlus3.y + j) * sizePlus3.z + k];
}

inline ivec3 ImplicitGrapher::getXYZLineIndex(const int& i, const int& j, const int& k){
    return xyzLineIndex[(i * sizePlus3.y + j) * sizePlus3.z + k];
}

void ImplicitGrapher::refactor(const ivec3& inputRadius) {
    radius = inputRadius;
    radiusPlusOne = radius + ivec3(1);
    size = radius * 2;
    sizePlus2 = size + ivec3(2);
    sizePlus3 = size + ivec3(3);
    defaultOffset = vec3(radiusPlusOne);
    maxSolutionCount = 3 * sizePlus3.x * sizePlus3.y * sizePlus3.z;
}

#endif

ImplicitGrapher graph(20);

class Graph : public Wallpaper{
public:

    GLuint mProgram;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "layout(location = " STRV(NORMAL_ATTRIBUTE_LOCATION) ") in vec3 normal;\n"
            "out vec3 vNormal;\n"
            "uniform mat4 mvp;\n"
            "void main() {\n"
            "    gl_Position = mvp*vec4(pos, 1.0);\n"
            "    vNormal = normal;\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "in vec3 vNormal;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); \n"
            "}\n";

    Graph();

    ~Graph();

    void render() override;

private:
};

Graph::Graph() : Wallpaper(){
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
}

Graph::~Graph(){
    glDeleteProgram(mProgram);
}

void Graph::render(){
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 50.0f * val - 50.0f));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> mvp = perspective * translation * rotation;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    graph.surfaceEquation = 1;
    graph.calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * framesRendered, 10, vec3(0.0f), 0.15f, false, false, &graph.vertices[0], graph.indices, graph.numIndices);

    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&graph.vertices[0].v);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormal), (const GLvoid*)&graph.vertices[0].n);
    glDrawElements(GL_TRIANGLES, graph.numIndices, GL_UNSIGNED_INT, graph.indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

    framesRendered++;
}

class Simulation : public Wallpaper{
public:
    int moleculeCount;
    int cbrtMoleculeCount;
    void setMoleculeCount(int n);
};

void Simulation::setMoleculeCount(int n){
    moleculeCount = n;
    cbrtMoleculeCount = cbrt(moleculeCount);
}

class BarnesHut : public Simulation{

};

class BruteForce : public Simulation{

};

class Naive : public Simulation{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    struct Molecule {
        vec3 position;
        vec3 velocity;
        vec3 positionFinal;
        vec3 sumForceInitial;
        vec3 sumForceFinal;
    };

    Molecule* molecules;

    bool seed(const float& radius);

    void simulate(const vec3& gravity);

    Naive();

    ~Naive();

    void render() override;

private:

    const float deltaTime = 0.02f;

    const float surfaceTension = 0.05f;

    float sphereRadius;

    float sphereRadiusSquared;

    float sphereRadiusPlusPointFive;

    float sphereRadiusPlusPointFiveSquared;

    const float efficiency = 1.0f;//0.995f;

    const float maxForce = 100000000.0f;

    const float maxForceSquared = maxForce * maxForce;

    const double maxForceDistance = 2.0;

    const double maxForceDistanceSquared = maxForceDistance * maxForceDistance;

    float particleRadius;

    int* numCellParticles;

    int* firstCellParticle;

    int* cellParticleIds;

    float pInvSpacing;

    int pNumX;

    int pNumY;

    int pNumZ;

    int pNumCells;

    void populateGrid(const int& dataIndex);

    void useGrid(const int& i, const int& dataIndex, int* const ids, int& count);

    float noPullBarrier(const float& x);

    float f(const float& x);

    inline void calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex);
};

Naive::Naive() : Simulation(){
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, v));

    seed(15.0f);
}

Naive::~Naive(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(1, mVB);
    delete molecules;
}

void Naive::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(
            10.0f * Vec3<float>(0.0f, 0.0f, 10.0f * val - 6.0f));
    Matrix4<float> translation2;
    for(int i = 0; i < moleculeCount; i++) {
        translation2 = translation2.Translation(
                Vec3<float>(molecules[i].position[0], molecules[i].position[1], molecules[i].position[2]));
        Matrix4<float> mvp = perspective * translation * translation2;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glBindVertexArray(mVBState);
        vec4 color = vec4(
                0.06125f * molecules[i].velocity.x + 0.5f,
                -0.06125f * molecules[i].velocity.y + 0.5f,
                -0.06125f * molecules[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    for(int i = 0; i < 5; i++){
        simulate(-accelerometerVector);
    }

    checkGlError("Renderer::render");
    framesRendered++;
}

void Naive::populateGrid(const int& dataIndex){
    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < moleculeCount; i++) {
        float x = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        numCellParticles[cellNr]++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += numCellParticles[i];
        firstCellParticle[i] = first;
    }
    firstCellParticle[pNumCells] = first; // guard

    // fill particles into cells

    for (int i = 0; i < moleculeCount; i++) {
        float x = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
        float y = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
        float z = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }
}

void Naive::useGrid(const int& i, const int& dataIndex, int* const ids, int& count){
    count = 0;
    float px = ((vec3*)&molecules[i])[dataIndex].x + sphereRadiusPlusPointFive;
    float py = ((vec3*)&molecules[i])[dataIndex].y + sphereRadiusPlusPointFive;
    float pz = ((vec3*)&molecules[i])[dataIndex].z + sphereRadiusPlusPointFive;

    int pxi = floor(px * pInvSpacing);
    int pyi = floor(py * pInvSpacing);
    int pzi = floor(pz * pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, pNumX - 1);
    int y1 = min(pyi + 1, pNumY - 1);
    int z1 = min(pzi + 1, pNumZ - 1);

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
                int first = firstCellParticle[cellNr];
                int last = firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = cellParticleIds[j];
                    if (id == i)
                        continue;
                    ids[count++] = id;
                }
            }
        }
    }
}

bool Naive::seed(const float& radius) {
    setMoleculeCount(125);
    molecules = new Molecule[moleculeCount];
    sphereRadius = radius;
    sphereRadiusSquared = sphereRadius * sphereRadius;
    sphereRadiusPlusPointFive = sphereRadius + 0.5f;
    sphereRadiusPlusPointFiveSquared = sphereRadiusPlusPointFive * sphereRadiusPlusPointFive;
    for (int i = 0; i < moleculeCount; i++) {
        int first = i - (i % (cbrtMoleculeCount * cbrtMoleculeCount));
        molecules[i].position.x = first / (cbrtMoleculeCount * cbrtMoleculeCount) - (cbrtMoleculeCount - 1) / 2.0f;
        first = i - first;
        molecules[i].position.y = (first - (first % cbrtMoleculeCount)) / cbrtMoleculeCount - (cbrtMoleculeCount - 1) / 2.0f;
        molecules[i].position.z = first % cbrtMoleculeCount - (cbrtMoleculeCount - 1) / 2.0f;
        molecules[i].velocity = vec3(0.0f, 0.000001f, 0.0f);
        molecules[i].positionFinal = molecules[i].position;
    }

    particleRadius = 0.5f;
    pInvSpacing = 1.0 / (maxForceDistance * 2.0 * particleRadius);//4.0 correlates to max distance of 2.0, 3.0 correlates to max distance of 1.5
    float width = 2.0f * sphereRadiusPlusPointFive;
    float height = 2.0f * sphereRadiusPlusPointFive;
    float depth = 2.0f * sphereRadiusPlusPointFive;
    pNumX = floor(width * pInvSpacing) + 1;
    pNumY = floor(height * pInvSpacing) + 1;
    pNumZ = floor(depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;
    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(moleculeCount, sizeof(int));

    populateGrid(0);

    return true;
}

float Naive::noPullBarrier(const float& x) {
    if (x > 1.0f) {
        return 0.0f;
    }
    return 1000.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
}

float Naive::f(const float& x) {
    if (x > maxForceDistance) {
        return 0.0f;
    }
    float _return = 500.0f * (x - 1.0f) * pow(x - 2.0f, 2.0f) / (x + 0.75f);
    if (x > 1.0f) {
        return surfaceTension * _return;
    } else {
        return _return;
    }
}

inline void Naive::calculateForce(const int& i, const vec3& gravity, vec3& force, const vec3& position, const int& dataIndex){
    force = gravity;
    int ids[1000]; // 700 may be high enough
    int count = 0;
    useGrid(i, dataIndex, ids, count);
    for (int j = 0; j < count; j++) {
        if (i == j) {
            continue;
        }
        vec3 difference = ((vec3*)&molecules[ids[j]])[dataIndex] - position;
        float r = distance(difference);
        float fOverR = f(r) / r;
        force += fOverR * difference;
    }
    float distanceFromOrigin = distance(position);
    if (distanceFromOrigin != 0.0f) {
        float forceAtPoint = noPullBarrier(-distanceFromOrigin + sphereRadiusPlusPointFive) / distanceFromOrigin;//if distanceFromOrigin = 0, then we plug in a value of 3.5, if distanceFromOrigin = 2.5, then we plug in a value of 1
        force += forceAtPoint * position;
    }
}

void Naive::simulate(const vec3& gravity) {
    for (int i = 0; i < moleculeCount; i++) {
        calculateForce(i, gravity, molecules[i].sumForceInitial, molecules[i].position, 0);
        molecules[i].positionFinal = molecules[i].position + deltaTime * (molecules[i].sumForceInitial * 0.5f * deltaTime + molecules[i].velocity);
        // Bug fix: Stop runaway particles
        float squaredDistance = dot(molecules[i].positionFinal, molecules[i].positionFinal);
        if(squaredDistance > sphereRadiusPlusPointFive * sphereRadiusPlusPointFive){
            molecules[i].positionFinal *= sqrt(sphereRadius * sphereRadius / squaredDistance);
        }
    }
    populateGrid(2);
    for (int i = 0; i < moleculeCount; i++) {
        calculateForce(i, gravity, molecules[i].sumForceFinal, molecules[i].positionFinal, 2);
        vec3 sumForceAverage = 0.5f * (molecules[i].sumForceInitial + molecules[i].sumForceFinal);
        molecules[i].position += deltaTime * (sumForceAverage * 0.5f * deltaTime + molecules[i].velocity);
        molecules[i].velocity += sumForceAverage * deltaTime;
        // Efficiency control
        if (efficiency != 1.0f) {
            vec3 product = molecules[i].velocity * efficiency;
            molecules[i].velocity = product;
        }
        //  Bug fix: stop runaway particles
        float squaredDistance = dot(molecules[i].position, molecules[i].position);
        if(squaredDistance > sphereRadiusSquared){
            molecules[i].position *= sqrt(sphereRadiusSquared / squaredDistance);
            molecules[i].velocity = 0.1f * molecules[i].position;
        }
    }
    populateGrid(0);
}

struct {
    const int width = 1000;
    const int height = 1000;
} canvas;

float simHeight = 3.0;
float cScale = canvas.height / simHeight;
float simWidth = canvas.width / cScale;
float simDepth = simHeight;  // Assuming the depth is the same as the height

const int FLUID_CELL = 0;
const int AIR_CELL = 1;
const int SOLID_CELL = 2;

float min(const float& a, const float& b)  // Updated function signature and parameter type
{
    if (a < b)
        return a;
    else
        return b;
}

// ----------------- start of simulator ------------------------------

class FlipFluid {
public:

    // fluid

    float density;
    int fNumX;
    int fNumY;
    int fNumZ;  // New variable for the z-axis
    float h;
    float fInvSpacing;
    int fNumCells;

    float* u;
    float* v;
    float* w;  // New variable for the z-axis velocity
    float* du;
    float* dv;
    float* dw;  // New variable for the z-axis velocity
    float* prevU;
    float* prevV;
    float* prevW;  // New variable for the z-axis velocity
    float* p;
    float* s;
    int* cellType;

    // particles

    int maxParticles;

    vec3* particlePos;
    vec3* particleColor;

    vec3* particleVel;

    float* particleDensity;
    float particleRestDensity;

    float particleRadius;
    float pInvSpacing;
    int pNumX;
    int pNumY;
    int pNumZ;  // New variable for the z-axis
    int pNumCells;

    int* numCellParticles;
    int* firstCellParticle;
    int* cellParticleIds;

    int numParticles;

    float dt = 1.0 / 60.0;
    float flipRatio = 0.9f;
    int numPressureIters = 50;
    int numParticleIters = 2;
    int frameNr = 0;
    float overRelaxation = 1.9f;
    bool compensateDrift = true;
    bool separateParticles = true;

    FlipFluid(const float& _density, const float& _width, const float& _height, const float& _depth, const float& _spacing, const float& _particleRadius, const int& _maxParticles);

private:
    void integrateParticles(const float& _dt, const vec3& _gravity);

    void pushParticlesApart(const float& _numIters);

    void handleParticleCollisions();

    void updateParticleDensity();

    void transferVelocities(const bool& _toGrid, const float& _flipRatio);

    void solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift);

    void updateParticleColors();

public:
    void simulate(const float& _dt, const vec3& _gravity, const float& _flipRatio, const int& _numPressureIters, const int& _numParticleIters, const float& _overRelaxation, const bool& _compensateDrift, const bool& _separateParticles);
};

FlipFluid* fluid = NULL;

FlipFluid::FlipFluid(const float& _density, const float& _width, const float& _height, const float& _depth, const float& _spacing, const float& _particleRadius, const int& _maxParticles) {

    // fluid properties

    density = _density;
    fNumX = floor(_width / _spacing) + 1;
    fNumY = floor(_height / _spacing) + 1;
    fNumZ = floor(_depth / _spacing) + 1;
    h = max(max(_width / fNumX, _height / fNumY), _depth / fNumZ);
    fInvSpacing = 1.0 / h;
    fNumCells = fNumX * fNumY * fNumZ;

    u = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the x-direction
    v = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the y-direction
    w = (float*)calloc(fNumCells, sizeof(float));         // Initialize fluid velocity in the z-direction
    du = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the x-direction
    dv = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the y-direction
    dw = (float*)calloc(fNumCells, sizeof(float));        // Initialize the change in fluid velocity in the z-direction
    prevU = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the x-direction
    prevV = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the y-direction
    prevW = (float*)calloc(fNumCells, sizeof(float));     // Initialize the previous fluid velocity in the z-direction
    p = (float*)calloc(fNumCells, sizeof(float));         // Initialize pressure
    s = (float*)calloc(fNumCells, sizeof(float));         // Initialize source term
    cellType = (int*)calloc(fNumCells, sizeof(int));        // Initialize cell type (fluid, air, solid)

    // particle properties

    maxParticles = _maxParticles;

    particlePos = (vec3*)calloc(maxParticles, sizeof(vec3));   // Initialize particle positions (x, y, z)
    particleColor = (vec3*)calloc(maxParticles, sizeof(vec3)); // Initialize particle color (for visualization)

    particleVel = (vec3*)calloc(maxParticles, sizeof(vec3));      // Initialize particle velocity (x, y, z)
    particleDensity = (float*)calloc(fNumCells, sizeof(float));         // Initialize particle density
    particleRestDensity = 0.0;                                            // Set the rest density of the particles

    particleRadius = _particleRadius;
    pInvSpacing = 1.0 / (2.2 * _particleRadius);
    pNumX = floor(_width * pInvSpacing) + 1;
    pNumY = floor(_height * pInvSpacing) + 1;
    pNumZ = floor(_depth * pInvSpacing) + 1;
    pNumCells = pNumX * pNumY * pNumZ;

    numCellParticles = (int*)calloc(pNumCells, sizeof(int));              // Initialize the number of particles in each cell
    firstCellParticle = (int*)calloc((pNumCells + 1), sizeof(int));       // Initialize the index of the first particle in each cell
    cellParticleIds = (int*)calloc(_maxParticles, sizeof(int));           // Initialize the particle ids

    numParticles = 0;                                                     // Set the initial number of particles to zero
}

void FlipFluid::integrateParticles(const float& _dt, const vec3& _gravity)
{
    for (int i = 0; i < numParticles; i++) {
        particleVel[i] += _dt * _gravity;
        particlePos[i] += particleVel[i] * _dt;
    }
}

void FlipFluid::pushParticlesApart(const float& _numIters)
{
    float colorDiffusionCoeff = 0.001;

    // count particles per cell

    for (int i = 0; i < pNumCells; i++)
        numCellParticles[i] = 0;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        numCellParticles[cellNr]++;
    }

    // partial sums

    int first = 0;

    for (int i = 0; i < pNumCells; i++) {
        first += numCellParticles[i];
        firstCellParticle[i] = first;
    }
    firstCellParticle[pNumCells] = first; // guard

    // fill particles into cells

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        int xi = clamp(floor(x * pInvSpacing), 0, pNumX - 1);
        int yi = clamp(floor(y * pInvSpacing), 0, pNumY - 1);
        int zi = clamp(floor(z * pInvSpacing), 0, pNumZ - 1);
        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
        firstCellParticle[cellNr]--;
        cellParticleIds[firstCellParticle[cellNr]] = i;
    }

    // push particles apart

    float minDist = 2.0 * particleRadius;
    float minDist2 = minDist * minDist;

    for (int iter = 0; iter < _numIters; iter++) {

        for (int i = 0; i < numParticles; i++) {
            float px = particlePos[i].x;
            float py = particlePos[i].y;
            float pz = particlePos[i].z;

            int pxi = floor(px * pInvSpacing);
            int pyi = floor(py * pInvSpacing);
            int pzi = floor(pz * pInvSpacing);
            int x0 = max(pxi - 1, 0);
            int y0 = max(pyi - 1, 0);
            int z0 = max(pzi - 1, 0);
            int x1 = min(pxi + 1, pNumX - 1);
            int y1 = min(pyi + 1, pNumY - 1);
            int z1 = min(pzi + 1, pNumZ - 1);

            for (int xi = x0; xi <= x1; xi++) {
                for (int yi = y0; yi <= y1; yi++) {
                    for (int zi = z0; zi <= z1; zi++) {
                        int cellNr = xi * pNumY * pNumZ + yi * pNumZ + zi;
                        int first = firstCellParticle[cellNr];
                        int last = firstCellParticle[cellNr + 1];
                        for (int j = first; j < last; j++) {
                            int id = cellParticleIds[j];
                            if (id == i)
                                continue;
                            float qx = particlePos[id].x;
                            float qy = particlePos[id].y;
                            float qz = particlePos[id].z;

                            float dx = qx - px;
                            float dy = qy - py;
                            float dz = qz - pz;
                            float d2 = dx * dx + dy * dy + dz * dz;
                            if (d2 > minDist2 || d2 == 0.0)
                                continue;
                            float d = sqrt(d2);
                            float s = 0.5 * (minDist - d) / d;
                            dx *= s;
                            dy *= s;
                            dz *= s;
                            particlePos[i].x -= dx;
                            particlePos[i].y -= dy;
                            particlePos[i].z -= dz;
                            particlePos[id].x += dx;
                            particlePos[id].y += dy;
                            particlePos[id] += dz;

                            // diffuse colors

                            for (int k = 0; k < 3; k++) {
                                float color0 = particleColor[i].v[k];
                                float color1 = particleColor[id].v[k];
                                float color = (color0 + color1) * 0.5;
                                particleColor[i].v[k] = color0 + (color - color0) * colorDiffusionCoeff;
                                particleColor[id].v[k] = color1 + (color - color1) * colorDiffusionCoeff;
                            }
                        }
                    }
                }
            }
        }
    }
}

void FlipFluid::handleParticleCollisions()
{
    float r = particleRadius;

    float minX = h + r;
    float maxX = (fNumX - 1) * h - r;
    float minY = h + r;
    float maxY = (fNumY - 1) * h - r;
    float minZ = h + r;
    float maxZ = (fNumZ - 1) * h - r;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        // wall collisions

        if (x < minX) {
            x = minX;
            particleVel[i].x = 0.0;
        }
        if (x > maxX) {
            x = maxX;
            particleVel[i].x = 0.0;
        }
        if (y < minY) {
            y = minY;
            particleVel[i].y = 0.0;
        }
        if (y > maxY) {
            y = maxY;
            particleVel[i].y = 0.0;
        }
        if (z < minZ) {
            z = minZ;
            particleVel[i].z = 0.0;
        }
        if (z > maxZ) {
            z = maxZ;
            particleVel[i].z = 0.0;
        }
        particlePos[i].x = x;
        particlePos[i].y = y;
        particlePos[i].z = z;
    }
}

void FlipFluid::updateParticleDensity()
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5 * h;

    float* d = particleDensity;

    for (int i = 0; i < fNumCells; i++)
        d[i] = 0.0;

    for (int i = 0; i < numParticles; i++) {
        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;

        x = clamp(x, h, (fNumX - 1) * h);
        y = clamp(y, h, (fNumY - 1) * h);
        z = clamp(z, h, (fNumZ - 1) * h);

        int x0 = floor((x - h2) * h1);
        float tx = ((x - h2) - x0 * h) * h1;
        int x1 = min(x0 + 1, fNumX - 2);

        int y0 = floor((y - h2) * h1);
        float ty = ((y - h2) - y0 * h) * h1;
        int y1 = min(y0 + 1, fNumY - 2);

        int z0 = floor((z - h2) * h1);
        float tz = ((z - h2) - z0 * h) * h1;
        int z1 = min(z0 + 1, fNumZ - 2);

        float sx = 1.0 - tx;
        float sy = 1.0 - ty;
        float sz = 1.0 - tz;

        if (x0 < fNumX && y0 < fNumY && z0 < fNumZ) d[x0 * (n * fNumZ) + y0 * fNumZ + z0] += sx * sy * sz;
        if (x1 < fNumX && y0 < fNumY && z0 < fNumZ) d[x1 * (n * fNumZ) + y0 * fNumZ + z0] += tx * sy * sz;
        if (x1 < fNumX && y1 < fNumY && z0 < fNumZ) d[x1 * (n * fNumZ) + y1 * fNumZ + z0] += tx * ty * sz;
        if (x0 < fNumX && y1 < fNumY && z0 < fNumZ) d[x0 * (n * fNumZ) + y1 * fNumZ + z0] += sx * ty * sz;
        if (x0 < fNumX && y0 < fNumY && z1 < fNumZ) d[x0 * (n * fNumZ) + y0 * fNumZ + z1] += sx * sy * tz;
        if (x1 < fNumX && y0 < fNumY && z1 < fNumZ) d[x1 * (n * fNumZ) + y0 * fNumZ + z1] += tx * sy * tz;
        if (x1 < fNumX && y1 < fNumY && z1 < fNumZ) d[x1 * (n * fNumZ) + y1 * fNumZ + z1] += tx * ty * tz;
        if (x0 < fNumX && y1 < fNumY && z1 < fNumZ) d[x0 * (n * fNumZ) + y1 * fNumZ + z1] += sx * ty * tz;
    }

    if (particleRestDensity == 0.0) {
        float sum = 0.0;
        int numFluidCells = 0;

        for (int i = 0; i < fNumCells; i++) {
            if (cellType[i] == FLUID_CELL) {
                sum += d[i];
                numFluidCells++;
            }
        }

        if (numFluidCells > 0)
            particleRestDensity = sum / numFluidCells;
    }
}

void FlipFluid::transferVelocities(const bool& _toGrid, const float& _flipRatio)
{
    int n = fNumY;
    float h1 = fInvSpacing;
    float h2 = 0.5f * h;

    if (_toGrid) {
        for (int i = 0; i < fNumCells; i++) {
            prevU[i] = u[i];
            prevV[i] = v[i];
            prevW[i] = w[i];
        }

        for (int i = 0; i < fNumCells; i++) {
            du[i] = 0.0;
            dv[i] = 0.0;
            dw[i] = 0.0;
            u[i] = 0.0;
            v[i] = 0.0;
            w[i] = 0.0;
        }

        for (int i = 0; i < fNumCells; i++)
            cellType[i] = s[i] == 0.0 ? SOLID_CELL : AIR_CELL;

        for (int i = 0; i < numParticles; i++) {
            float x = particlePos[i].x;
            float y = particlePos[i].y;
            float z = particlePos[i].z;

            int xi = clamp(floor(x * h1), 0, fNumX - 1);
            int yi = clamp(floor(y * h1), 0, fNumY - 1);
            int zi = clamp(floor(z * h1), 0, fNumZ - 1);

            int cellNr = xi * (n * fNumZ) + yi * fNumZ + zi;
            if (cellType[cellNr] == AIR_CELL)
                cellType[cellNr] = FLUID_CELL;
        }
    }

    for (int component = 0; component < 3; component++) {
        float dx = (component == 0) ? 0.0 : h2;
        float dy = (component == 1) ? 0.0 : h2;
        float dz = (component == 2) ? 0.0 : h2;

        float* f = (component == 0) ? u : ((component == 1) ? v : w);
        float* prevF = (component == 0) ? prevU : ((component == 1) ? prevV : prevW);
        float* d = (component == 0) ? du : ((component == 1) ? dv : dw);

        for (int i = 0; i < numParticles; i++) {
            float x = particlePos[i].x;
            float y = particlePos[i].y;
            float z = particlePos[i].z;

            x = clamp(x, h, (fNumX - 1) * h);
            y = clamp(y, h, (fNumY - 1) * h);
            z = clamp(z, h, (fNumZ - 1) * h);

            int x0 = min(floor((x - dx) * h1), fNumX - 2);
            float tx = ((x - dx) - x0 * h) * h1;
            int x1 = min(x0 + 1, fNumX - 2);

            int y0 = min(floor((y - dy) * h1), fNumY - 2);
            float ty = ((y - dy) - y0 * h) * h1;
            int y1 = min(y0 + 1, fNumY - 2);

            int z0 = min(floor((z - dz) * h1), fNumZ - 2);
            float tz = ((z - dz) - z0 * h) * h1;
            int z1 = min(z0 + 1, fNumZ - 2);

            float sx = 1.0 - tx;
            float sy = 1.0 - ty;
            float sz = 1.0 - tz;

            float d0 = sx * sy * sz;
            float d1 = tx * sy * sz;
            float d2 = tx * ty * sz;
            float d3 = sx * ty * sz;
            float d4 = sx * sy * tz;
            float d5 = tx * sy * tz;
            float d6 = tx * ty * tz;
            float d7 = sx * ty * tz;

            int nr0 = x0 * (n * fNumZ) + y0 * fNumZ + z0;
            int nr1 = x1 * (n * fNumZ) + y0 * fNumZ + z0;
            int nr2 = x1 * (n * fNumZ) + y1 * fNumZ + z0;
            int nr3 = x0 * (n * fNumZ) + y1 * fNumZ + z0;
            int nr4 = x0 * (n * fNumZ) + y0 * fNumZ + z1;
            int nr5 = x1 * (n * fNumZ) + y0 * fNumZ + z1;
            int nr6 = x1 * (n * fNumZ) + y1 * fNumZ + z1;
            int nr7 = x0 * (n * fNumZ) + y1 * fNumZ + z1;

            if (_toGrid) {
                float pv = particleVel[i].v[component];
                f[nr0] += pv * d0; d[nr0] += d0;
                f[nr1] += pv * d1; d[nr1] += d1;
                f[nr2] += pv * d2; d[nr2] += d2;
                f[nr3] += pv * d3; d[nr3] += d3;
                f[nr4] += pv * d4; d[nr4] += d4;
                f[nr5] += pv * d5; d[nr5] += d5;
                f[nr6] += pv * d6; d[nr6] += d6;
                f[nr7] += pv * d7; d[nr7] += d7;
            }
            else {
                int offset = (component == 0) ? (n * fNumZ) : ((component == 1) ? fNumZ : 1);

                float valid0 = (cellType[nr0] != AIR_CELL || cellType[nr0 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid1 = (cellType[nr1] != AIR_CELL || cellType[nr1 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid2 = (cellType[nr2] != AIR_CELL || cellType[nr2 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid3 = (cellType[nr3] != AIR_CELL || cellType[nr3 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid4 = (cellType[nr4] != AIR_CELL || cellType[nr4 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid5 = (cellType[nr5] != AIR_CELL || cellType[nr5 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid6 = (cellType[nr6] != AIR_CELL || cellType[nr6 - offset] != AIR_CELL) ? 1.0 : 0.0;
                float valid7 = (cellType[nr7] != AIR_CELL || cellType[nr7 - offset] != AIR_CELL) ? 1.0 : 0.0;

                float v = particleVel[i].v[component];
                float d = valid0 * d0 + valid1 * d1 + valid2 * d2 + valid3 * d3 +
                          valid4 * d4 + valid5 * d5 + valid6 * d6 + valid7 * d7;

                if (d > 0.0) {
                    float picV = (valid0 * d0 * f[nr0] + valid1 * d1 * f[nr1] + valid2 * d2 * f[nr2] + valid3 * d3 * f[nr3] +
                                  valid4 * d4 * f[nr4] + valid5 * d5 * f[nr5] + valid6 * d6 * f[nr6] + valid7 * d7 * f[nr7]) / d;
                    float corr = (valid0 * d0 * (f[nr0] - prevF[nr0]) + valid1 * d1 * (f[nr1] - prevF[nr1]) +
                                  valid2 * d2 * (f[nr2] - prevF[nr2]) + valid3 * d3 * (f[nr3] - prevF[nr3]) +
                                  valid4 * d4 * (f[nr4] - prevF[nr4]) + valid5 * d5 * (f[nr5] - prevF[nr5]) +
                                  valid6 * d6 * (f[nr6] - prevF[nr6]) + valid7 * d7 * (f[nr7] - prevF[nr7])) / d;
                    float flipV = v + corr;

                    particleVel[i].v[component] = (1.0 - _flipRatio) * picV + _flipRatio * flipV;
                }
            }
        }

        if (_toGrid) {
            for (int i = 0; i < fNumCells; i++) {
                if (d[i] > 0.0)
                    f[i] /= d[i];
            }

            // Restore solid cells
            for (int i = 0; i < fNumX; i++) {
                for (int j = 0; j < fNumY; j++) {
                    for (int k = 0; k < fNumZ; k++) {
                        int solid = cellType[i * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL;
                        if (solid || (i > 0 && cellType[(i - 1) * (n * fNumZ) + j * fNumZ + k] == SOLID_CELL))
                            u[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                        if (solid || (j > 0 && cellType[i * (n * fNumZ) + (j - 1) * fNumZ + k] == SOLID_CELL))
                            v[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                        if (solid || (k > 0 && cellType[i * (n * fNumZ) + j * fNumZ + (k - 1)] == SOLID_CELL))
                            w[i * (n * fNumZ) + j * fNumZ + k] = 0.0;
                    }
                }
            }
        }
    }
}

void FlipFluid::solveIncompressibility(const int& _numIters, const float& _dt, const float& _overRelaxation, const bool& _compensateDrift = true) {
    for (int i = 0; i < fNumCells; i++) {
        p[i] = 0.0;
        prevU[i] = u[i];
        prevV[i] = v[i];
        prevW[i] = w[i];
    }

    int n = fNumY;
    float cp = density * h / _dt;

    for (int iter = 0; iter < _numIters; iter++) {
        for (int i = 1; i < fNumX - 1; i++) {
            for (int j = 1; j < fNumY - 1; j++) {
                for (int k = 1; k < fNumZ - 1; k++) {

                    if (cellType[i * (n * fNumZ) + j * fNumZ + k] != FLUID_CELL)
                        continue;

                    int center = i * (n * fNumZ) + j * fNumZ + k;
                    int left = (i - 1) * (n * fNumZ) + j * fNumZ + k;
                    int right = (i + 1) * (n * fNumZ) + j * fNumZ + k;
                    int bottom = i * (n * fNumZ) + (j - 1) * fNumZ + k;
                    int top = i * (n * fNumZ) + (j + 1) * fNumZ + k;
                    int back = i * (n * fNumZ) + j * fNumZ + (k - 1);
                    int front = i * (n * fNumZ) + j * fNumZ + (k + 1);

                    float sx0 = s[left];
                    float sx1 = s[right];
                    float sy0 = s[bottom];
                    float sy1 = s[top];
                    float sz0 = s[back];
                    float sz1 = s[front];
                    float s = sx0 + sx1 + sy0 + sy1 + sz0 + sz1;

                    if (s == 0.0)
                        continue;

                    float div = u[right] - u[center] +
                                v[top] - v[center] +
                                w[front] - w[center];

                    if (particleRestDensity > 0.0 && _compensateDrift) {
                        float _k = 1.0;
                        float compression = particleDensity[i * (n * fNumZ) + j * fNumZ + k] - particleRestDensity;
                        if (compression > 0.0)
                            div = div - _k * compression;
                    }

                    float p = -div / s;
                    p *= _overRelaxation;
                    this->p[center] += cp * p;

                    u[center] -= sx0 * p;
                    u[right] += sx1 * p;
                    v[center] -= sy0 * p;
                    v[top] += sy1 * p;
                    w[center] -= sz0 * p;
                    w[front] += sz1 * p;
                }
            }
        }
    }
}

void FlipFluid::updateParticleColors()
{
    float h1 = fInvSpacing;

    for (int i = 0; i < numParticles; i++) {
        float s = 0.01;

        particleColor[i].r = clamp(particleColor[i].r - s, 0.0, 1.0);
        particleColor[i].g = clamp(particleColor[i].g - s, 0.0, 1.0);
        particleColor[i].b = clamp(particleColor[i].b + s, 0.0, 1.0);

        float x = particlePos[i].x;
        float y = particlePos[i].y;
        float z = particlePos[i].z;
        int xi = clamp(floor(x * h1), 1, fNumX - 1);
        int yi = clamp(floor(y * h1), 1, fNumY - 1);
        int zi = clamp(floor(z * h1), 1, fNumZ - 1);
        int cellNr = xi * fNumY * fNumZ + yi * fNumZ + zi;

        float d0 = particleRestDensity;

        if (d0 > 0.0) {
            float relDensity = particleDensity[cellNr] / d0;
            if (relDensity < 0.7) {
                float s = 0.8;
                particleColor[i].r = s;
                particleColor[i].g = s;
                particleColor[i].b = 1.0;
            }
        }
    }
}

void FlipFluid::simulate(const float& _dt, const vec3& _gravity, const float& _flipRatio, const int& _numPressureIters, const int& _numParticleIters, const float& _overRelaxation, const bool& _compensateDrift, const bool& _separateParticles)
{
    int numSubSteps = 1;
    float sdt = _dt / numSubSteps;

    for (int step = 0; step < numSubSteps; step++) {
        this->integrateParticles(sdt, _gravity);
        if (_separateParticles)
            this->pushParticlesApart(_numParticleIters);
        this->handleParticleCollisions();
        this->transferVelocities(true, _flipRatio);
        this->updateParticleDensity();
        this->solveIncompressibility(_numPressureIters, sdt, _overRelaxation, _compensateDrift);
        this->transferVelocities(false, _flipRatio);
    }

    this->updateParticleColors();
}

class PicFlip : public Simulation{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

    void render() override;

    PicFlip();

    ~PicFlip();

public:

    static void simulate(const vec3& acceleration){
        fluid->simulate(
                fluid->dt, acceleration, fluid->flipRatio, fluid->numPressureIters, fluid->numParticleIters,
                fluid->overRelaxation, fluid->compensateDrift, fluid->separateParticles);
        fluid->frameNr++;
    }

    static void setupScene(){
        int res = 15;

        float tankHeight = 1.0f * simHeight;
        float tankWidth = 1.0f * simWidth;
        float tankDepth = 1.0f * simDepth;
        float h = tankHeight / res;
        float density = 1000.0f;

        float relWaterHeight = 1.0f;
        float relWaterWidth = 0.6f;
        float relWaterDepth = 1.0f;

        // dam break

        // compute number of particles

        float r = 0.3 * h;    // particle radius w.r.t. cell size
        float dx = 2.0 * r;
        float dy = sqrt(3.0) / 2.0 * dx;
        float dz = 2.0 * r;

        int numX = floor((relWaterWidth * tankWidth - 2.0 * h - 2.0 * r) / dx);
        int numY = floor((relWaterHeight * tankHeight - 2.0 * h - 2.0 * r) / dy);
        int numZ = floor((relWaterDepth * tankDepth - 2.0 * h - 2.0 * r) / dz);
        int maxParticles = numX * numY * numZ;

        // create fluid

        fluid = new FlipFluid(density, tankWidth, tankHeight, tankDepth, h, r, maxParticles);

        // create particles

        fluid->numParticles = numX * numY * numZ;
        int p = 0;
        for (int i = 0; i < numX; i++) {
            for (int j = 0; j < numY; j++) {
                for (int k = 0; k < numZ; k++) {
                    fluid->particlePos[p++] = vec3(
                            h + r + dx * i + (j % 2 == 0 ? 0.0 : r),
                            h + r + dy * j,
                            h + dz * k
                    );
                }
            }
        }

        // setup grid cells for tank

        int nx = fluid->fNumX;
        int ny = fluid->fNumY;
        int nz = fluid->fNumZ;

        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                for (int k = 0; k < nz; k++) {
                    float s = 1.0;    // fluid
                    if (i == 0 || i == nx - 1 || j == 0 || j == ny - 1 || k == 0 || k == nz - 1)
                        s = 0.0;    // solid
                    fluid->s[(i * ny + j) * nz + k] = s;
                }
            }
        }
    }

private:
};

PicFlip::PicFlip() : Simulation(){
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, v));

    setupScene();
}

PicFlip::~PicFlip(){

}

void PicFlip::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(
            Vec3<float>(0.0f, 0.0f, val * 100.0f - 60.0f) - Vec3<float>(15.f));
    Matrix4<float> translation2;
    for(int i = 0; i < fluid->numParticles; i++) {
        translation2 = translation2.Translation(
                10.0f * Vec3<float>(fluid->particlePos[i].x, fluid->particlePos[i].y, fluid->particlePos[i].z));
        Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
        Matrix4<float> mvp = perspective * translation * translation2;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glBindVertexArray(mVBState);
        vec4 color = vec4(
                0.5f * fluid->particleVel[i].x + 0.5f,
                -0.5f * fluid->particleVel[i].y + 0.5f,
                -0.5f * fluid->particleVel[i].z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    }

    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);

    for(int i = 0; i < 2; i++){
        if(distance(accelerometerVector) == 0.0f){
            simulate(vec3(0.0f, -9.81f, 0.0f));
        }else{
            simulate(-accelerometerVector);
        }
    }

    checkGlError("Renderer::render");
    framesRendered++;
}

Wallpaper* wallpaper = nullptr;

// ----------------------------------------------------------------------------

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_livewallpaper05_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

// ----------------------------------------------------------------------------

#if !defined(DYNAMIC_ES3)
static GLboolean gl3stubInit() { return GL_TRUE; }
#endif

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_init(JNIEnv *env, jobject thiz, jint visualization) {
    Wallpaper::printGlString("Version", GL_VERSION);
    Wallpaper::printGlString("Vendor", GL_VENDOR);
    Wallpaper::printGlString("Renderer", GL_RENDERER);
    Wallpaper::printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        switch(visualization){
            case 0:
                wallpaper = new Box();
                break;
            case 1:
                wallpaper = new Naive();
                break;
            case 2:
                wallpaper = new PicFlip();
                break;
            case 3:
                wallpaper = new Triangle();
                break;
            case 4:
                wallpaper = new Graph();
                break;
        }
        ALOGV("Using OpenGL ES 3.0 renderer");
    } else if (strstr(versionStr, "OpenGL ES 2.")) {
        //g_renderer = createES2Renderer();
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_resize(JNIEnv *env, jobject thiz,
                                                                    jint width, jint height) {
    if (wallpaper) {
        wallpaper->width = width;
        wallpaper->height = height;
        glViewport(0, 0, width, height);
        wallpaper->calculatePerspective();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat value) {
    if (wallpaper) {
        wallpaper->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        wallpaper->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        wallpaper->val = value;
        wallpaper->render();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_sendData(JNIEnv *env, jobject thiz,
                                                                      jfloat value) {
    wallpaper->val = value;
}