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

#define POS_ATTRIB 0

// returns true if a GL error occurred
bool checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

GLuint createShader(GLenum shaderType, const char* src) {
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

GLuint createProgram(const char* vtxSrc, const char* fragSrc) {
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

static void printGlString(const char* name, GLenum s) {
    const char* v = (const char*)glGetString(s);
    ALOGV("GL %s: %s\n", name, v);
}

struct Vertex {
    GLfloat pos[3];
};

class Wallpaper{
public:

    int framesRendered = 0;

    EGLContext mEglContext;

    float val = 0.0f;

    EGLint width;

    EGLint height;

    vec3 accelerometerVector;

    vec4 rotationVector;

    Wallpaper();

    ~Wallpaper();

    virtual bool initialize();

    virtual void render();

private:

};

Wallpaper::Wallpaper() : framesRendered(0), mEglContext(eglGetCurrentContext()){

}

Wallpaper::~Wallpaper(){
    if (eglGetCurrentContext() != mEglContext) return;
}

bool Wallpaper::initialize(){

};

void Wallpaper::render(){

};

class Box : public Wallpaper{
public:

    GLuint mProgram;

    GLuint mVB[1];

    GLuint mVBState;

    const char* VERTEX_SHADER =
            "#version 310 es\n"
            "layout(location = " STRV(POS_ATTRIB) ") in vec3 pos;\n"
            "uniform mat4 mvp;\n"
            "out vec4 vColor;\n"
            "void main() {\n"
            "    gl_Position = mvp * vec4(pos, 1.0);\n"
            "}\n";

    const char* FRAGMENT_SHADER =
            "#version 310 es\n"
            "precision mediump float;\n"
            "uniform vec4 color;\n"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = color;\n"
            "}\n";

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

    Box();

    ~Box();

    bool initialize();

    void render();

private:
};

Box::Box() : Wallpaper(){
    initialize();
}

Box::~Box(){
    glDeleteProgram(mProgram);
    glDeleteVertexArrays(1, &mVBState);
    glDeleteBuffers(1, mVB);
}

bool Box::initialize() {
    mProgram = 0;
    mVBState = 0;
    mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!mProgram) return false;

    glGenBuffers(1, mVB);
    glBindBuffer(GL_ARRAY_BUFFER, mVB[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BOX), &BOX[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &mVBState);
    glBindVertexArray(mVBState);

    glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid*)offsetof(Vertex, pos));

    return true;
}

void Box::render(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POS_ATTRIB);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> perspective;
    perspective.SetPerspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(Vec3<float>(0.0f, 0.0f, val * 1.0f - 3.0f)));
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(rotationVector));
    Matrix4<float> MVP = perspective * translation * rotation;
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&MVP.cell);
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

    glDisableVertexAttribArray(POS_ATTRIB);

    checkGlError("Renderer::render");
    framesRendered++;
}

class Graph : Wallpaper{

};

class Simulation : Wallpaper{

};

class BarnesHut : Simulation{

};

class BruteForce : Simulation{

};

class Naive : Simulation{

};

class PicFlip : Simulation{

};

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
Java_com_example_livewallpaper05_MainActivity_00024Companion_init(JNIEnv *env, jobject thiz) {
    printGlString("Version", GL_VERSION);
    printGlString("Vendor", GL_VENDOR);
    printGlString("Renderer", GL_RENDERER);
    printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        wallpaper = new Box();
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
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w) {
    if (wallpaper) {
        wallpaper->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        wallpaper->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        wallpaper->render();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_MainActivity_00024Companion_sendData(JNIEnv *env, jobject thiz,
                                                                      jfloat value) {
    ((Box*)wallpaper)->val = value;
}