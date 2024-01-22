
#define DEBUG 1

#define LOG_TAG "GLES3JNI"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#if DEBUG
#define ALOGV(...) \
  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define ALOGV(...)
#endif

#define STR(s) #s
#define STRV(s) STR(s)

#define YES true
#define NO false

#include "nlohmann/json.hpp"
#include "vectors.cpp"
#include "VertexAttributesArray.cpp"
#include "PositionXYZ.cpp"
#include "PositionXYZNormalXYZ.cpp"
#include "PositionXYZColorRGB.cpp"
#include "View.cpp"
#include "TriangleStripObject.cpp"
#include "VertexArrayObject.cpp"
#include "Sphere.cpp"
#include "SphereView.cpp"
#include "Cube.cpp"
#include "CubeView.cpp"
#include "RGBCubeView.cpp"
#include "TriangleView.cpp"
#include "TriangleWithNormalsView.cpp"
#include "ComputeShader.cpp"
#include "Computation.cpp"
#include "Simulation.cpp"
#include "SimpleNBodySimulation.cpp"
#include "SimpleNBodySimulationView.cpp"
#include "EnvironmentMap.cpp"
#include "CubeMap.cpp"
#include "SphereMap.cpp"
#include "ImplicitGrapher.cpp"
#include "GraphView.cpp"
#include "Texture.cpp"
#include "NaiveSimulation.cpp"
#include "NaiveSimulationView.cpp"
#include "NaiveSimulationFluidSurfaceView.cpp"
#include "FlipFluid.cpp"
#include "PicFlipView.cpp"
#include "CubeMapView.cpp"
#include "SphereMapView.cpp"
#include "TextureView.cpp"
#include "RenderToTextureView.cpp"
#include "RenderToCubeMapView.cpp"
#include "FBO.cpp"
#include "CubeMapFBO.cpp"
#include "DrawWithFragmentShaderView.cpp"
#include "SphereWithRefractionView.cpp"
#include "SphereWithReflectionView.cpp"
#include "SphereWithFresnelEffectView.cpp"

using std::string;
using nlohmann::json;

View* view = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_init(JNIEnv *env, jobject thiz, jstring JSON) {
    View::printGlString("Version", GL_VERSION);
    View::printGlString("Vendor", GL_VENDOR);
    View::printGlString("Renderer", GL_RENDERER);
    View::printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")) {
        json visualizationJSON = json::parse(View::jstringToString(env, JSON));
        string type = visualizationJSON["type"];
        if(view){
            free(view);
        }
        if(type == "box"){
            //view = new RGBCubeView();
            //view = new TriangleView();
            //view = new TriangleWithNormalsView();
            //view = new CubeMapView();
            //view = new SphereMapView();
            //view = new TextureView();
            //view = new RenderToTextureView();
            //view = new RenderToCubeMapView();
            //view = new DrawWithFragmentShaderView();
            //view = new SphereView();
            //view = new CubeView();
            //view = new SphereWithReflectionView();
            //view = new SphereWithRefractionView();
            //view = new SphereWithFresnelEffectView();
            //view = new SimpleNBodySimulationView();
            view = new NaiveSimulationFluidSurfaceView(1000, 40, 20.0f, true);
        }else if(type == "naive"){
            view = new NaiveSimulationView(1000, 15.0f);
        }else if(type == "picflip"){
            view = new PicFlipView();
        }else if(type == "triangle"){
            //view = new TriangleView();
            //view = new SphereMapView();
            view = new SphereWithFresnelEffectView(Texture::MANDELBROT, 2048);
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
        ALOGI("Orientation = %s\n", to_string((int)orientation).c_str());
        view->calculatePerspectiveSetViewport(60.0f);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat value) {
    if (view) {
        view->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        view->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        view->zoom = value;
        view->render();
        view->incrementFrameCount();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_sendData(JNIEnv *env, jobject thiz, jfloat value) {
    view->zoom = value;
}