#include <jni.h>
#include <string>
#include <cstdlib>
#include <string>
#include <ctime>
#include <stack>
#include <android/log.h>
#include <cmath>
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include "vectors.h"
#include "cyCodeBase-master/cyMatrix.h"
#include "nlohmann/json.hpp"
#include "Vertex.cpp"
#include "VertexNormal.cpp"

using cy::Matrix4;
using cy::Vec3;
using std::min;
using std::max;
using std::string;
using namespace nlohmann;

#define DEBUG 1

#define LOG_TAG "GLES3JNI"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
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

#include "Wallpaper.cpp"
#include "Box.cpp"
#include "Triangle.cpp"
#include "ImplicitGrapher.cpp"

ImplicitGrapher implicitGrapher(20);

#include "Graph.cpp"
#include "Simulation.cpp"
#include "BarnesHut.cpp"
#include "BruteForce.cpp"
#include "Naive.cpp"

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

#include "FlipFluid.cpp"

FlipFluid* fluid = NULL;

#include "PicFlip.cpp"

Wallpaper* wallpaper = nullptr;

// Function to convert jstring to std::string
std::string jstringToString(JNIEnv *env, jstring jStr) {
    if (jStr == nullptr) {
        return ""; // Handle null jstring gracefully
    }

    const char *chars = env->GetStringUTFChars(jStr, nullptr);
    std::string result(chars);
    env->ReleaseStringUTFChars(jStr, chars);

    return result;
}

// ----------------------------------------------------------------------------

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_livewallpaper05_PreviewActivity_stringFromJNI(
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
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_init(JNIEnv *env, jobject thiz, jstring JSON) {
    Wallpaper::printGlString("Version", GL_VERSION);
    Wallpaper::printGlString("Vendor", GL_VENDOR);
    Wallpaper::printGlString("Renderer", GL_RENDERER);
    Wallpaper::printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        json visualizationJSON = json::parse(jstringToString(env, JSON));
        string type = visualizationJSON["type"];
        if(wallpaper){
            free(wallpaper);
        }
        if(type == "box"){
            wallpaper = new Box();
        }else if(type == "naive"){
            wallpaper = new Naive();
        }else if(type == "picflip"){
            wallpaper = new PicFlip();
        }else if(type == "triangle"){
            wallpaper = new Triangle();
        }else if(type == "graph"){
            wallpaper = new Graph(visualizationJSON["settings"]);
        }
        json rgba = visualizationJSON["background_color"];
        wallpaper->backgroundColor = vec4(rgba["r"], rgba["g"], rgba["b"], rgba["a"]) / 255.0f;
        ALOGV("Using OpenGL ES 3.0 renderer");
    } else if (strstr(versionStr, "OpenGL ES 2.")) {
        //g_renderer = createES2Renderer();
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_resize(JNIEnv *env, jobject thiz,
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
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat value) {
    if (wallpaper) {
        wallpaper->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        wallpaper->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        wallpaper->val = value;
        wallpaper->render();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_sendData(JNIEnv *env, jobject thiz,
                                                                         jfloat value) {
    wallpaper->val = value;
}