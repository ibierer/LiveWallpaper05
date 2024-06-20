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

#include <jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <string>
#include "nlohmann/json.hpp"
#include "PositionXYZ.h"
#include "PositionXYZNormalXYZ.h"
#include "cyCodeBase-master/cyMatrix.h"
#include "vectors.cpp"
#include "VertexAttributesArray.cpp"
#include "PositionXYZ.cpp"
#include "PositionXYZNormalXYZ.cpp"
#include "PositionXYZColorRGB.cpp"
#include "View.cpp"
#include "Texture.cpp"
#include "FBO.cpp"
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
#include "NaiveSimulation.cpp"
#include "NaiveSimulationView.cpp"
#include "NaiveSimulationFluidSurfaceView.cpp"
#include "FlipFluid.cpp"
#include "PicFlipView.cpp"
#include "CubeMapView.cpp"
#include "SphereMapView.cpp"
#include "ShaderToyView.cpp"
#include "TextureView.cpp"
#include "RenderToTextureView.cpp"
#include "RenderToCubeMapView.cpp"
#include "CubeMapFBO.cpp"
#include "DrawWithFragmentShaderView.cpp"
#include "SphereWithRefractionView.cpp"
#include "SphereWithReflectionView.cpp"
#include "SphereWithFresnelEffectView.cpp"
#include "Graph2View.cpp"
#include "LinearithmicNBodySimulationView.h"

using std::string;
using nlohmann::json;

View* view[2] = {nullptr, nullptr}; // Separate pointers for Activity and Wallpaper Service
Matrix3<float> rotationMatrix;

extern "C"
JNIEXPORT void JNICALL
Java_com_vizbox4d_PreviewActivity_00024Companion_init(JNIEnv *env, jobject thiz, jstring JSON, jint mode) {
    View::printGlString("Version", GL_VERSION);
    View::printGlString("Vendor", GL_VENDOR);
    View::printGlString("Renderer", GL_RENDERER);
    View::printGlString("Extensions", GL_EXTENSIONS);

    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")) {
        if(view[mode]){
            free(view[mode]);
        }

        json visualizationJSON = json::parse(View::jstringToString(env, JSON));
        string backgroundTexture = visualizationJSON["background_texture"];

        if (backgroundTexture == "ms_paint_colors") {
            View::backgroundTexture = Texture::DefaultImages::MS_PAINT_COLORS;
        } else if(backgroundTexture == "mandelbrot") {
            View::backgroundTexture = Texture::DefaultImages::MANDELBROT;
        }

        string visualizationType = visualizationJSON["visualization_type"];

        if(visualizationType == "simulation"){
            string simulation = visualizationJSON["simulation_type"];
            if(simulation == "nbody"){
                //view[mode] = new RGBCubeView();
                //view[mode] = new TriangleView();
                //view[mode] = new TriangleWithNormalsView();
                //view[mode] = new CubeMapView();
                //view[mode] = new SphereMapView();
                //view[mode] = new TextureView();
                //view[mode] = new RenderToTextureView();
                //view[mode] = new RenderToCubeMapView();
                //view[mode] = new DrawWithFragmentShaderView();
                //view[mode] = new SphereView();
                //view[mode] = new CubeView();
                //view[mode] = new SphereWithReflectionView();
                //view[mode] = new SphereWithFresnelEffectView();
                //view[mode] = new SimpleNBodySimulationView();
                //view[mode] = new ShaderToyView();
                view[mode] = new LinearithmicNBodySimulationView();
            }else if(simulation == "naive"){
                int particleCount = visualizationJSON["particle_count"];
                bool fluidSurface = visualizationJSON["fluid_surface"];
                bool referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
                bool smoothSphereSurface = visualizationJSON["smooth_sphere_surface"];
                //view[mode] = new NaiveSimulationView(particleCount, 15.0f, referenceFrameRotates, gravity);
                view[mode] = new NaiveSimulationFluidSurfaceView(particleCount, fluidSurface, 40, 20.0f, referenceFrameRotates, smoothSphereSurface);
            }else if(simulation == "picflip"){
                bool referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
                view[mode] = new PicFlipView(referenceFrameRotates);
            }
        }else if(visualizationType == "other"){
            view[mode] = new SphereWithFresnelEffectView();
        }else if(visualizationType == "graph"){
            string equation = visualizationJSON["equation"];
            ImplicitGrapher::convertPiSymbol(equation);
            string syntaxCheck = ImplicitGrapher::checkEquationSyntax(equation);
            bool referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
            bool vectorPointsPositive = visualizationJSON["vector_points_positive"];
            if(syntaxCheck == "") {
                view[mode] = new Graph2View(equation, 40, referenceFrameRotates, vectorPointsPositive);
            }else{
                view[mode] = new Graph2View("", 40, referenceFrameRotates, vectorPointsPositive);
            }
        }

        view[mode]->backgroundIsSolidColor = visualizationJSON["background_is_solid_color"];
        json rgba = visualizationJSON["background_color"];
        view[mode]->backgroundColor = vec4((float)rgba["r"], (float)rgba["g"], (float)rgba["b"], 255.0f) / 255.0f;
        ALOGV("Using OpenGL ES 3.0 renderer");
    } else if (strstr(versionStr, "OpenGL ES 2.")) {
        //g_renderer = createES2Renderer();
    } else {
        ALOGE("Unsupported OpenGL ES version");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vizbox4d_PreviewActivity_00024Companion_resize(JNIEnv *env, jobject thiz, jint width, jint height, jint mode) {
    if (view[mode]) {
        if (view[mode]->initialWidth == 0 || view[mode]->initialHeight == 0) {
            view[mode]->initialWidth = width;
            view[mode]->initialHeight = height;
        }
        view[mode]->width = width;
        view[mode]->height = height;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vizbox4d_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat linear_acc_x, jfloat linear_acc_y, jfloat linear_acc_z, jfloat distance, jfloat field_of_view, jfloat gravity, jfloat efficiency, jboolean flip_normals, jint orientation, jint mode) {
    if (view[mode]) {
        view[mode]->accelerometerVector = vec3(acc_x, acc_y, acc_z);
        view[mode]->linearAccelerationVector = vec3(linear_acc_x, linear_acc_y, linear_acc_z);
        view[mode]->rotationVector = vec4(rot_x, rot_y, rot_z, rot_w);
        //if(view[mode]->getFrameCount() == 0){
        //    rotationMatrix = quaternionTo3x3(Vec4<float>(rot_x, rot_y, rot_z, rot_w));
        //    for(int i = 0; i < 9; i++){
        //        view[mode]->incrementalRotationMatrix[i] = (float)(i % 4 == 0);
        //    }
        //}else{
        //    Matrix3<float> currentRotationMatrix = quaternionTo3x3(Vec4<float>(rot_x, rot_y, rot_z, rot_w));
        //    Matrix3<float> incrementalRotationMatrix = rotationMatrix.GetInverse() * currentRotationMatrix;
        //    for(int i = 0; i < 9; i++){
        //        view[mode]->incrementalRotationMatrix[i] = incrementalRotationMatrix[i];
        //    }
        //    rotationMatrix = currentRotationMatrix;
        //}
        //ALOGI("view[mode]->incrementalRotationMatrix = %s %s %s\n", view[mode]->incrementalRotationMatrix.m[0].str().c_str(), view[mode]->incrementalRotationMatrix.m[1].str().c_str(), view[mode]->incrementalRotationMatrix.m[2].str().c_str());
        view[mode]->distanceToOrigin = distance;
        view[mode]->maxViewAngle = field_of_view;
        view[mode]->gravity = gravity;
        view[mode]->efficiency = efficiency;
        ImplicitGrapher::vectorPointsPositive = flip_normals;
        view[mode]->orientation = orientation;
        //ALOGI("field_of_view = %s\n", to_string(field_of_view).c_str());
        view[mode]->calculatePerspectiveSetViewport(view[mode]->maxViewAngle, view[mode]->zNear, view[mode]->zFar);
        view[mode]->render();
        view[mode]->incrementFrameCount();
    }
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_vizbox4d_PreviewActivity_00024Companion_getScreenBuffer(JNIEnv *env, jobject thiz, jint mode) {
    // Capture the screen buffer
    vector<unsigned char> pixels = vector<unsigned char>(view[mode]->width * view[mode]->height * 4);
    glReadPixels(0, 0, view[mode]->width, view[mode]->height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
    // Convert the pixel data to a Java byte array
    jbyteArray byteArray = env->NewByteArray(pixels.size());
    env->SetByteArrayRegion(byteArray, 0, pixels.size(), (jbyte*)pixels.data());
    return byteArray;
}