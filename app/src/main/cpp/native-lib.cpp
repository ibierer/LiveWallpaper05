
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

bool aDigit(char character){
    return character >= '0' && character <= '9';
}
bool aCharacter(char character){
    return
            character == 'x' ||
            character == 'y' ||
            character == 'z' ||
            character == 't' ||
            character == 'e' ||
            character == 5;
}
bool anOperator(char character){
    return
            character == '^' ||
            character == '/' ||
            character == '*' ||
            character == '-' ||
            character == '+';
}
string checkEquationSyntax(const string& editable){
    string syntaxError = "";
    int i;
    bool containsEqualSign = false;
    int parenthesisCountLeft = 0;
    int parenthesisCountRight = 0;
    bool improperParenthesisLeft = false;
    bool improperParenthesisRight = false;
    for(i = 0; i < editable.length(); i++){
        if(editable.at(i) == '('){
            if(containsEqualSign){
                parenthesisCountRight++;
            }else{
                parenthesisCountLeft++;
            }
        }else if(editable.at(i) == ')'){
            if(containsEqualSign){
                parenthesisCountRight--;
            }else{
                parenthesisCountLeft--;
            }
        }else if(editable.at(i) == '='){
            if(containsEqualSign){
                syntaxError = "Syntax Error:  contains more than one equal sign.";
                return syntaxError;
            }else{
                containsEqualSign = true;
            }
        }
        if(parenthesisCountLeft < 0){
            improperParenthesisLeft = true;
        }else if(parenthesisCountRight < 0){
            improperParenthesisRight = true;
        }
    }
    if(!containsEqualSign){
        syntaxError = "Syntax Error:  lacks an equal sign.";
        return syntaxError;
    }
    if(improperParenthesisLeft || improperParenthesisRight){
        syntaxError = "Syntax Error:  improper parenthesis on ";
        if(!improperParenthesisLeft){
            syntaxError += "right.";
        }else if(improperParenthesisRight){
            syntaxError += "both sides.";
        }else{
            syntaxError += "left.";
        }
        return syntaxError;
    }
    if(parenthesisCountLeft != 0 || parenthesisCountRight != 0){
        syntaxError = "Syntax Error:  invalid parenthesis count on ";
        if(parenthesisCountLeft == 0){
            syntaxError += "right.";
        }else if(parenthesisCountRight != 0){
            syntaxError += "both sides.";
        }else{
            syntaxError += "left.";
        }
        return syntaxError;
    }

    //Check for expressions
    bool termOnLeft = false;
    bool termOnRight = false;
    bool passedEqualSign = false;
    for(int i = 0; i < editable.length() && !termOnRight; i++){
        if(aDigit(editable.at(i)) || aCharacter(editable.at(i))){
            if(passedEqualSign){
                termOnRight = true;
            }else{
                termOnLeft = true;
            }
        }else if(editable.at(i) == '='){
            passedEqualSign = true;
        }
    }
    if(!termOnLeft || !termOnRight){
        syntaxError = "Syntax Error:  no expression on ";
        if(termOnLeft){
            syntaxError += "right side.";
        }else if(!termOnRight){
            syntaxError += "either side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Improper term I
    bool badTermOnLeft = false;
    bool badTermOnRight = false;
    passedEqualSign = false;
    for(int i = 0; i < editable.length() - 1 && !badTermOnRight; i++){
        if(
                aCharacter(editable.at(i)) &&
                (
                        editable.at(i + 1) == '.' ||
                        aDigit(editable.at(i + 1))
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }else if(editable.at(i) == '='){
            passedEqualSign = true;
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  improper term on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Convert to noSpaces
    string noSpaces = "";
    for(int i = 0; i < editable.length(); i++){
        if(editable.at(i) != ' '){
            noSpaces += editable.at(i);
        }
    }

    //Improper term II
    passedEqualSign = false;
    for(int i = 0; i < noSpaces.length() - 1 && !badTermOnRight; i++){
        if(noSpaces.at(i) == '='){
            passedEqualSign = true;
        }
        if(
                (
                        noSpaces.at(i) == '(' &&
                        (
                                noSpaces.at(i + 1) == ')' ||
                                (
                                        anOperator(noSpaces.at(i + 1)) &&
                                        noSpaces.at(i + 1) != '-'
                                )
                        )
                ) || (
                        (
                                noSpaces.at(i) == '(' ||
                                anOperator(noSpaces.at(i))
                        ) &&
                        noSpaces.at(i + 1) == ')'
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  improper term on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Improper Decimal
    passedEqualSign = false;
    for(int i = 0; i < noSpaces.length() - 1 && !badTermOnRight; i++){
        if(noSpaces.at(i) == '='){
            passedEqualSign = true;
        }
        if(
                (
                        noSpaces.at(i) == '.' &&
                        !aDigit(noSpaces.at(i + 1))
                ) ||
                (
                        i == noSpaces.length() - 2 &&
                        noSpaces.at(noSpaces.length() - 1) == '.'
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  improper decimal on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    //Invalid Operator
    passedEqualSign = false;
    for(int i = 0; i < noSpaces.length() - 1 && !badTermOnRight; i++){
        if(noSpaces.at(i) == '='){
            passedEqualSign = true;
        }
        if(
                (
                        i == noSpaces.length() - 2 &&
                        anOperator(noSpaces.at(noSpaces.length() - 1))
                ) || (
                        anOperator(noSpaces.at(i)) &&
                        (
                                noSpaces.at(i + 1) == ')' ||
                                noSpaces.at(i + 1) == '=' ||
                                (
                                        i == 0 &&
                                        noSpaces.at(i) != '-'
                                )
                        )
                ) || (
                        (
                                noSpaces.at(i) == '(' ||
                                noSpaces.at(i) == '='
                        ) && (
                                anOperator(noSpaces.at(i + 1)) &&
                                noSpaces.at(i + 1) != '-'
                        )
                )
                ){
            if(!passedEqualSign){
                badTermOnLeft = true;
            }else{
                badTermOnRight = true;
            }
        }
    }
    if(badTermOnLeft || badTermOnRight){
        syntaxError = "Syntax Error:  invalid operator on ";
        if(!badTermOnLeft){
            syntaxError += "right side.";
        }else if(badTermOnRight){
            syntaxError += "each side.";
        }else{
            syntaxError += "left side.";
        }
        return syntaxError;
    }

    syntaxError = "";
    return syntaxError;
}

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
        string type = visualizationJSON["visualization_type"];
        if(view){
            free(view);
        }
        if(type == "simulation"){
            string simulation = visualizationJSON["simulation_type"];
            if(simulation == "naive"){
                string fluidSurface = visualizationJSON["fluid_surface"];
                int particleCount = visualizationJSON["particle_count"];
                if(fluidSurface == "true"){
                    string gravityOn = visualizationJSON["gravity_on"];
                    string referenceFrameRotates = visualizationJSON["reference_frame_rotates"];
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
                    view = new NaiveSimulationFluidSurfaceView(particleCount, 40, 20.0f, referenceFrameRotates == "true", gravityOn == "true");
                }else{
                    view = new NaiveSimulationView(particleCount, 15.0f);
                }
            }else if(simulation == "nbody"){
                view = new SimpleNBodySimulationView();
            }else if(simulation == "picflip"){
                view = new PicFlipView();
            }
        }else if(type == "graph"){
            string equation = visualizationJSON["equation"];
            ImplicitGrapher::convertPiSymbol(equation);
            string syntaxCheck = checkEquationSyntax(equation);
            ALOGI("Result of syntax check: %s\n", syntaxCheck.c_str());
            if(syntaxCheck == "") {
                view = new GraphView(equation);
            }else{
                view = new GraphView("");
            }
            ALOGI("equation = %s\n", to_string(visualizationJSON["equation"]).c_str());
        }else if(type == "other"){
            view = new SphereWithFresnelEffectView(Texture::MANDELBROT, 2048);
        }
        json rgba = visualizationJSON["background_color"];
        uint r = rgba["r"];
        uint g = rgba["g"];
        uint b = rgba["b"];
        // print visualizationJSON to logcat with tag "Livewallpaper05"
        //view->backgroundColor = vec4(rgba["r"], rgba["g"], rgba["b"], rgba["a"]) / 255.0f;
        view->backgroundColor = vec4((float)r, (float)g, (float)b, 255) / 255.0f;
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
        ALOGI("Orientation_update = %s\n", to_string(orientation).c_str());
        view->calculatePerspectiveSetViewport(60.0f, view->zNear, view->zFar);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_step(JNIEnv *env, jobject thiz, jfloat acc_x, jfloat acc_y, jfloat acc_z, jfloat rot_x, jfloat rot_y, jfloat rot_z, jfloat rot_w, jfloat linear_acc_x, jfloat linear_acc_y, jfloat linear_acc_z, jfloat value) {
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
Java_com_example_livewallpaper05_PreviewActivity_00024Companion_sendData(JNIEnv *env, jobject thiz, jfloat value) {
    view->zoom = value;
}