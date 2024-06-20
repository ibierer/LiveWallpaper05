//
// Created by Immanuel Bierer on 2/18/2024.
//

#include "Graph2View.h"

using std::min;
using std::max;

Graph2View::Graph2View(const string &equation, const int &graphSize, const bool &referenceFrameRotates, const bool& vectorPointsPositive) : View() {
    this->referenceFrameRotates = referenceFrameRotates;

    sphereMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
    if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
        sphereMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
        sphereMap = SphereMap(Texture::DefaultImages::MANDELBROT, 2048, 2048, this);
    }
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(sphereMap.environmentTriangleVertices) / sizeof(PositionXYZ));

    graphNormalMapProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_NORMAL_MAP_FRAGMENT_SHADER.c_str());
    graphFluidSurfaceProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_FRAGMENT_SHADER.c_str());

    implicitGrapher = ImplicitGrapher(ivec3(graphSize));
    implicitGrapher.vectorPointsPositive = vectorPointsPositive;
    if(equation != "") {
        ImplicitGrapher::surfaceEquation = 0;
        ImplicitGrapher::memoryEquations[ImplicitGrapher::surfaceEquation][1] = equation;
        ImplicitGrapher::processEquation(ImplicitGrapher::surfaceEquation);
        ImplicitGrapher::numOfEquationsInMemory++;
    }
}

Graph2View::~Graph2View(){
    glDeleteProgram(mProgram);
}

void Graph2View::render(){
    Matrix4<float> translation, rotation, model, view, projection, mvp, cameraTransformation, inverseViewProjection;
    Matrix3<float> normalMatrix;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    if (getFrameCount() == 0) {
        fbo = FBO(
                (void *) new Texture(GL_RGBA, width, height, 0, GL_LINEAR),
                YES,
                NO);
    }

    enum Material {
        MERCURY,
        WATER,
        BUBBLE
    } material = WATER;
    float indexOfRefraction;
    float reflectivity;
    int twoSidedRefraction;
    switch (material) {
        case MERCURY:
            indexOfRefraction = 1.0f;
            reflectivity = 1.0f;
            twoSidedRefraction = NO;
            break;
        case WATER:indexOfRefraction = 4.0f / 3.0f;
            reflectivity = -1.0f;
            twoSidedRefraction = YES;
            break;
        case BUBBLE:indexOfRefraction = 3.0f / 4.0f;
            reflectivity = 0.0f;
            twoSidedRefraction = NO;
            break;
    }

    float distanceToCenter = 100.0f * distanceToOrigin;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -distanceToCenter));
    normalMatrix = referenceFrameRotates ? rotation.GetSubMatrix3().GetInverse() : normalMatrix.Identity();

    if(getFrameCount() == 0 || ImplicitGrapher::hasTimeVariable[ImplicitGrapher::surfaceEquation] || vectorPointsPositive != ImplicitGrapher::vectorPointsPositive){
        ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, implicitGrapher.vectorPointsPositive, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);
        vectorPointsPositive = ImplicitGrapher::vectorPointsPositive;
    }

    // Render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
    glDrawBuffers(1, fbo.drawBuffers);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        // Prepare model-view-projection matrix
        model = model.Translation(Vec3<float>(0.0f, 0.0f, 0.0f));
        view = referenceFrameRotates ? translation : translation * rotation;
        projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
        mvp = projection * view * model;

        // Render graph
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glUseProgram(graphNormalMapProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(graphNormalMapProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glUniformMatrix3fv(
                glGetUniformLocation(graphNormalMapProgram, "normalMatrix"),
                1,
                GL_FALSE,
                (GLfloat *) &normalMatrix);
        glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
        glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE,
                              sizeof(PositionXYZNormalXYZ),
                              (const GLvoid *) &ImplicitGrapher::vertices[0].p);
        glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE,
                              sizeof(PositionXYZNormalXYZ),
                              (const GLvoid *) &ImplicitGrapher::vertices[0].n);
        glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT,
                       ImplicitGrapher::indices);
        glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
        glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
        glCullFace(GL_BACK);
    }

    // Render to default frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(mProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
        glEnable(GL_CULL_FACE);

        // Prepare model-view-projection matrix
        model = model.Translation(Vec3<float>(0.0f, 0.0f, 0.0f));
        view = referenceFrameRotates ? translation : translation * rotation;
        projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
        mvp = projection * view * model;
        cameraTransformation = rotation.GetInverse() * translation * rotation * model;

        // Render graph
        glUseProgram(graphFluidSurfaceProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(graphFluidSurfaceProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glUniformMatrix4fv(
                glGetUniformLocation(graphFluidSurfaceProgram, "viewTransformation"),
                1,
                GL_FALSE,
                (GLfloat *) &cameraTransformation);
        glUniformMatrix3fv(
                glGetUniformLocation(graphFluidSurfaceProgram, "normalMatrix"),
                1,
                GL_FALSE,
                (GLfloat *) &normalMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
        glUniform1i(glGetUniformLocation(graphFluidSurfaceProgram, "environmentTexture"),
                    0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
        glUniform1i(glGetUniformLocation(graphFluidSurfaceProgram, "image"), 1);
        glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "reflectivity"),
                    reflectivity);
        glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "indexOfRefraction"),
                    indexOfRefraction);
        glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "inverseIOR"),
                    1.0f / indexOfRefraction);
        glUniform1i(glGetUniformLocation(graphFluidSurfaceProgram, "twoSidedRefraction"),
                    twoSidedRefraction);
        glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "screenWidth"), initialWidth);
        glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "screenHeight"), initialHeight);
        glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
        glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
        glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE,
                              sizeof(PositionXYZNormalXYZ),
                              (const GLvoid *) &ImplicitGrapher::vertices[0].p);
        glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE,
                              sizeof(PositionXYZNormalXYZ),
                              (const GLvoid *) &ImplicitGrapher::vertices[0].n);
        glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT,
                       ImplicitGrapher::indices);
        glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
        glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        if(!backgroundIsSolidColor) {
            // Render sphere map
            glUseProgram(sphereMapProgram);
            glUniformMatrix4fv(
                    glGetUniformLocation(sphereMapProgram, "inverseViewProjection"),
                    1,
                    GL_FALSE,
                    (GLfloat *) &inverseViewProjection);
            glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
            glActiveTexture(GL_TEXTURE1);
            glUniform1i(glGetUniformLocation(sphereMapProgram, "environmentTexture"), 1);
            environmentTriangleVAO.drawArrays();
        }
    }
}
