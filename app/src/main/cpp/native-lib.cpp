
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
#include "LinearithmicNBodySimulation.cpp"
#include "LinearithmicNBodySimulationView.cpp"
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
#include "Graph2View.cpp"
#include "LinearithmicNBodySimulationView.h"

using std::string;
using nlohmann::json;

View *view = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_init(JNIEnv *env, jobject thiz,
                                                                     jstring JSON) {
    View::printGlString("Version", GL_VERSION);
    View::printGlString("Vendor", GL_VENDOR);
    View::printGlString("Renderer", GL_RENDERER);
    View::printGlString("Extensions", GL_EXTENSIONS);

    const char *versionStr = (const char *) glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")) {
        if (view) {
            free(view);
        }

        json visualizationJSON = json::parse(View::jstringToString(env, JSON));

        string backgroundTexture = visualizationJSON["background_texture"];
        if (backgroundTexture == "ms_paint_colors") {
            View::backgroundTexture = Texture::DefaultImages::MS_PAINT_COLORS;
        } else if (backgroundTexture == "mandelbrot") {
            View::backgroundTexture = Texture::DefaultImages::MANDELBROT;
        }

        string visualizationType = visualizationJSON["visualization_type"];
        if (visualizationType == "simulation") {
            string simulation = visualizationJSON["simulation_type"];
            if (simulation == "nbody") {
                //view = new SimpleNBodySimulationView();
                view = new LinearithmicNBodySimulationView();
                //view = new RenderToCubeMapView();
            } else if (simulation == "naive") {
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
                //view = new LinearithmicNBodySimulationView();
                //view = new NaiveSimulationView(particleCount, 15.0f, referenceFrameRotates == "true", gravity);
                int particleCount = visualizationJSON["particle_count"];
                string fluidSurface = visualizationJSON["fluid_surface"];
                float gravity = visualizationJSON["gravity"];
                string referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
                string smoothSphereSurface = visualizationJSON["smooth_sphere_surface"];
                view = new NaiveSimulationFluidSurfaceView(particleCount, fluidSurface == "true",
                                                           40, 20.0f,
                                                           referenceFrameRotates == "true", gravity,
                                                           smoothSphereSurface == "true");
            } else if (simulation == "picflip") {
                float gravity = visualizationJSON["gravity"];
                string referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
                view = new PicFlipView(referenceFrameRotates == "true", gravity);
            }
        } else if (visualizationType == "other") {
            view = new SphereWithFresnelEffectView(Texture::MANDELBROT, 2048);
        } else if (visualizationType == "graph") {
            string equation = visualizationJSON["equation"];
            ImplicitGrapher::convertPiSymbol(equation);
            string syntaxCheck = ImplicitGrapher::checkEquationSyntax(equation);
            string referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
            string vectorPointsPositive = visualizationJSON["vector_points_positive"];
            if (syntaxCheck == "") {
                //view = new GraphView(equation);
                view = new Graph2View(equation, 40, referenceFrameRotates == "true",
                                      vectorPointsPositive == "true");
            } else {
                //view = new GraphView("");
                view = new Graph2View("", 40, referenceFrameRotates == "true",
                                      vectorPointsPositive == "true");
            }
        }
        view->backgroundIsSolidColor = visualizationJSON["background_is_solid_color"] == "true";
        json rgba = visualizationJSON["background_color"];
        view->backgroundColor =
                vec4((float) rgba["r"], (float) rgba["g"], (float) rgba["b"], 255.0f) / 255.0f;

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
                                                                       jint width, jint height,
                                                                       jint orientation) {
    if (view) {
        if (view->initialWidth == 0 || view->initialHeight == 0) {
            view->initialWidth = width;
            view->initialHeight = height;
        }
        view->width = width;
        view->height = height;
        view->orientation = orientation;
        //ALOGI("Orientation_update = %s\n", to_string(orientation).c_str());
        view->calculatePerspectiveSetViewport(60.0f, view->zNear, view->zFar);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz,
                                                                     jfloat acc_x, jfloat acc_y,
                                                                     jfloat acc_z, jfloat rot_x,
                                                                     jfloat rot_y, jfloat rot_z,
                                                                     jfloat rot_w,
                                                                     jfloat linear_acc_x,
                                                                     jfloat linear_acc_y,
                                                                     jfloat linear_acc_z,
                                                                     jfloat value) {
    if (view) {
        view->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        view->linearAccelerationVector = vec3(linear_acc_x, linear_acc_y, linear_acc_z);
        view->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        view->zoom = value;
        view->render();
        view->incrementFrameCount();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_sendData(JNIEnv *env, jobject thiz,
                                                                         jfloat value) {
    view->zoom = value;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_getScreenBuffer(JNIEnv *env,
                                                                                jobject thiz) {
    // Capture the screen buffer
    vector<unsigned char> pixels = vector<unsigned char>(view->width * view->height * 4);
    glReadPixels(0, 0, view->width, view->height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    // Convert the pixel data to a Java byte array
    jbyteArray byteArray = env->NewByteArray(pixels.size());
    env->SetByteArrayRegion(byteArray, 0, pixels.size(), (jbyte *) pixels.data());

    return byteArray;
}