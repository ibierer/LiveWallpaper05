//
// Created by Immanuel Bierer on 12/28/2023.
//

#include "CubeView.h"

CubeView::CubeView() : View() {
    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    //cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
}

CubeView::~CubeView(){
    glDeleteProgram(mProgram);
}

using std::min;
using std::max;

vec3 reflect2(const vec3 I, const vec3 N, const float dotNI) {
    return I - 2.0 * dotNI * N;
}

vec3 refract2(const vec3 I, const vec3 N, const float eta, const float dotNI) {
    float k = 1.0 - eta * eta * (1.0 - dotNI * dotNI);
    if (k < 0.0)
        return reflect2(I, N, dotNI);
    else
        return eta * I - (eta * dotNI + sqrt(k)) * N;
}

float intersectCube(vec3 rayOrigin, vec3 rayDirection, vec3 box_lower, vec3 box_upper, vec3& hitNormal) {
   rayOrigin = rayOrigin + 0.001f * rayDirection; // Apply a b
   float tx_min; // t-value of min value of x within c
   float tx_max; // t-value of max value of x within c
   float ty_min; // t-value of min value of y within c
   float ty_max; // t-value of max value of y within c
   float tz_min; // t-value of min value of z within c
   float tz_max; // t-value of max value of z within c
   float min_tx; // min t-value where x is within c
   float max_tx; // max t-value where x is within c
   float min_ty; // min t-value where y is within c
   float max_ty; // max t-value where y is within c
   float min_tz; // min t-value where z is within c
   float max_tz; // max t-value where z is within c
   if(rayDirection.x == 0.0f){
       if(rayOrigin.x < box_lower.x || box_upper.x < rayOrigin.x){
           return -1.0f;
       }
       tx_min = -1000000000000000000000000.0f;
       tx_max =  1000000000000000000000000.0f;
   }else{
       tx_min = (box_lower.x - rayOrigin.x) / rayDirection.x;
       tx_max = (box_upper.x - rayOrigin.x) / rayDirection.x;
   }
   if(rayDirection.y == 0.0f){
       if(rayOrigin.y < box_lower.y || box_upper.y < rayOrigin.y){
           return -1.0f;
       }
       ty_min = -1000000000000000000000000.0f;
       ty_max =  1000000000000000000000000.0f;
   }else{
       ty_min = (box_lower.y - rayOrigin.y) / rayDirection.y;
       ty_max = (box_upper.y - rayOrigin.y) / rayDirection.y;
   }
   if(rayDirection.z == 0.0f){
       if(rayOrigin.z < box_lower.z || box_upper.z < rayOrigin.z){
           return -1.0f;
       }
       tz_min = -1000000000000000000000000.0f;
       tz_max =  1000000000000000000000000.0f;
   }else{
       tz_min = (box_lower.z - rayOrigin.z) / rayDirection.z;
       tz_max = (box_upper.z - rayOrigin.z) / rayDirection.z;
   }
   min_tx = min(tx_min, tx_max);
   max_tx = max(tx_min, tx_max);
   min_ty = min(ty_min, ty_max);
   max_ty = max(ty_min, ty_max);
   min_tz = min(tz_min, tz_max);
   max_tz = max(tz_min, tz_max);
   float tMin = max(max(min_tx, min_ty), min_tz);
   float tMax = min(min(max_tx, max_ty), max_tz);
   if(tMin < tMax){
       hitNormal = vec3(0.0f);
       if(tMin < 0.0f){ // rayOrigin is inside c
           if(tMax == max_tx){
               if(max_tx == tx_max) hitNormal.x = 1.0f; else hitNormal.x = -1.0f;
           }else if(tMax == max_ty){
               if(max_ty == ty_max) hitNormal.y = 1.0f; else hitNormal.y = -1.0f;
           }else{ // if(tMax == max_t
               if(max_tz == tz_max) hitNormal.z = 1.0f; else hitNormal.z = -1.0f;
           }
           return tMax;
       }else{ // rayOrigin is outside c
           if(tMin == min_tx){
               if(min_tx == tx_min) hitNormal.x = -1.0f; else hitNormal.x = 1.0f;
           }else if(tMin == min_ty){
               if(min_ty == ty_min) hitNormal.y = -1.0f; else hitNormal.y = 1.0f;
           }else{ // if(tMin == min_t
               if(min_tz == tz_min) hitNormal.z = -1.0f; else hitNormal.z = 1.0f;
           }
           return tMin;
       }
   }else{
       return -1.0f;
   }
}

void CubeView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -20.0f * distanceToOrigin));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

    glUseProgram(mProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(mProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);

    //cubeVAO.drawArrays();

    float radius = 1.0f;

    PositionXYZ vertices[] = {
            PositionXYZ(vec3(-radius, -radius, radius)),
            PositionXYZ(vec3(radius, -radius, radius)),
            PositionXYZ(vec3(-radius, radius, radius)),
            PositionXYZ(vec3(radius, radius, radius)),
            PositionXYZ(vec3(-radius, radius, -radius)),
            PositionXYZ(vec3(radius, radius, -radius)),
            PositionXYZ(vec3(-radius, -radius, -radius)),
            PositionXYZ(vec3(radius, -radius, -radius)),
            PositionXYZ(vec3(radius, -radius, -radius)),
            PositionXYZ(vec3(radius, radius, -radius)),
            PositionXYZ(vec3(radius, radius, -radius)),
            PositionXYZ(vec3(radius, radius, radius)),
            PositionXYZ(vec3(radius, -radius, -radius)),
            PositionXYZ(vec3(radius, -radius, radius)),
            PositionXYZ(vec3(-radius, -radius, -radius)),
            PositionXYZ(vec3(-radius, -radius, radius)),
            PositionXYZ(vec3    (-radius, radius, -radius)),
            PositionXYZ(vec3(-radius, radius, radius)),

            PositionXYZ(vec3(2.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(2.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(2.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(2.0f, 0.0f, 0.0f)),
            PositionXYZ(vec3(2.0f, 0.0f, 0.0f))
    };
    float IOR = 0.75f;
    float inverseIOR = 1.0f / IOR;
    vec3 box_lower = vec3(-radius, -radius, -radius);
    vec3 box_upper = vec3( radius,  radius,  radius);
    vec3 p = vec3(2.0f, 1.0f, 0.0f);
    vec3 p2 = vec3(0.0f, 0.0f, 0.0f);
    vec3 direction = p2 - p;
    vec3 hitNormal;
    vec3 d = normalize(direction);
    int index = 19;
    float t = intersectCube(p, d, box_lower, box_upper, hitNormal);
    {
        if(t > 0.0f){ // Entering c
            ALOGI("CubeView %s\n", "point A");
            vertices[index].p = p + t * d;
            float dotNI = dot(hitNormal, d);
            vec3 refractedRay = normalize(refract2(d, hitNormal, IOR, dotNI));
            for(int i = 0; i < 3; i++){
                ALOGI("CubeView %s\n", "point B");
                t = intersectCube(vertices[index].p, refractedRay, box_lower, box_upper, hitNormal);
                if(t > 0.0f){
                    ALOGI("CubeView %s\n", "point C");
                    hitNormal = -hitNormal;
                    vertices[index + 1].p = vertices[index].p + t * refractedRay; index++;
                    dotNI = dot(hitNormal, refractedRay);
                    refractedRay = normalize(refract2(refractedRay, hitNormal, inverseIOR, dotNI));
                }else{
                    break;
                }
            }
        }else{ // No intersection with c
            //outColor = Texture(environmentTexture, d);
        }
    }

    const int numVertices = sizeof(vertices) / sizeof(PositionXYZ);
    PositionXYZ* _vertices = (PositionXYZ*)malloc(numVertices * sizeof(PositionXYZ));
    for(int i = 0; i < numVertices; i++) {
        ((PositionXYZ*)_vertices)[i] = vertices[i];
    }
    VertexArrayObject cubeVAO = VertexArrayObject(_vertices, numVertices);
    glBindVertexArray(cubeVAO.getArrayObjectId());
    glDrawArrays(GL_LINES, 0, numVertices);
    glBindVertexArray(0);
}
