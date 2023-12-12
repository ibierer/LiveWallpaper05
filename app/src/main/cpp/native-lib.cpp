
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

#include "nlohmann/json.hpp"
#include "vectors.cpp"
#include "Vertex.cpp"
#include "VertexNormal.cpp"
#include "Wallpaper.cpp"
#include "Box.cpp"
#include "Triangle.cpp"
#include "TriangleWithNormals.cpp"
#include "ImplicitGrapher.cpp"
#include "Graph.cpp"
#include "Simulation.cpp"
#include "BarnesHut.cpp"
#include "BruteForce.cpp"
#include "Naive.cpp"
#include "FlipFluid.cpp"
#include "PicFlip.cpp"
#include "CubeMap.cpp"
#include "CubeMapView.cpp"

using std::string;
using nlohmann::json;

Wallpaper* wallpaper = nullptr;

#if !defined(DYNAMIC_ES3)
static GLboolean gl3stubInit() {
    return GL_TRUE;
}
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
        json visualizationJSON = json::parse(Wallpaper::jstringToString(env, JSON));
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
            //wallpaper = new Triangle();
            //wallpaper = new TriangleWithNormals();
            wallpaper = new CubeMapView();
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
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_resize(JNIEnv *env, jobject thiz, jint width, jint height, jint orientation) {
    if (wallpaper) {
        wallpaper->width = width;
        wallpaper->height = height;
        wallpaper->orientation = orientation;
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
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_sendData(JNIEnv *env, jobject thiz, jfloat value) {
    wallpaper->val = value;
}