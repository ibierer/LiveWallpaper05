
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
#include "View.cpp"
#include "BoxView.cpp"
#include "TriangleView.cpp"
#include "TriangleWithNormalsView.cpp"
#include "ImplicitGrapher.cpp"
#include "GraphView.cpp"
#include "SimulationView.cpp"
#include "BarnesHut.cpp"
#include "BruteForce.cpp"
#include "NaiveView.cpp"
#include "FlipFluid.cpp"
#include "PicFlipView.cpp"
#include "EnvironmentMap.cpp"
#include "CubeMap.cpp"
#include "CubeMapView.cpp"
#include "SphereMap.cpp"
#include "SphereMapView.cpp"
#include "Texture.cpp"
#include "TextureView.cpp"
#include "RenderToTextureView.cpp"
#include "FBO.cpp"
#include "RenderToCubeMapView.cpp"

using std::string;
using nlohmann::json;

View* view = nullptr;

#if !defined(DYNAMIC_ES3)
static GLboolean gl3stubInit() {
    return GL_TRUE;
}
#endif

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_init(JNIEnv *env, jobject thiz, jstring JSON) {
    View::printGlString("Version", GL_VERSION);
    View::printGlString("Vendor", GL_VENDOR);
    View::printGlString("Renderer", GL_RENDERER);
    View::printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
        json visualizationJSON = json::parse(View::jstringToString(env, JSON));
        string type = visualizationJSON["type"];
        if(view){
            free(view);
        }
        if(type == "box"){
            //view = new BoxView();
            //view = new TriangleWithNormalsView();
            //view = new CubeMapView();
            //view = new SphereMapView();
            //view = new TextureView();
            //view = new RenderToTextureView();
            view = new RenderToCubeMapView();
        }else if(type == "naive"){
            view = new NaiveView();
        }else if(type == "picflip"){
            view = new PicFlipView();
        }else if(type == "triangle"){
            view = new TriangleView();
        }else if(type == "graph"){
            view = new GraphView(visualizationJSON["settings"]);
        }
        json rgba = visualizationJSON["background_color"];
        view->backgroundColor = vec4(rgba["r"], rgba["g"], rgba["b"], rgba["a"]) / 255.0f;
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
    if (view) {
        view->width = width;
        view->height = height;
        view->orientation = orientation;
        view->calculatePerspective(60.0f);
        glViewport(0, 0, width, height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat value) {
    if (view) {
        view->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        view->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        view->val = value;
        view->render();
        view->incrementFrameCount();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_sendData(JNIEnv *env, jobject thiz, jfloat value) {
    view->val = value;
}