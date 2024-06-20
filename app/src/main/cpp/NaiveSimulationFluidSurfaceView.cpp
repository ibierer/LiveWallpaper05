//
// Created by Immanuel Bierer on 1/15/2024.
//

#include "NaiveSimulationFluidSurfaceView.h"

using std::min;
using std::max;

bool sphereClipsGraph;
float fOfXYZFluidSurface(vec3 _, NaiveSimulation& sim) {
    _ -= ImplicitGrapher::offset;
    //return 1.0f - dot(_, _);
    //return 48.0f - dot(_, _);
    //return 52.0f - dot(_.xy, _.xy);

    /*if (
            abs(_.x) > (ImplicitGrapher::offset.x - 0.01f) ||
            abs(_.y) > (ImplicitGrapher::offset.y - 0.01f) ||
            abs(_.z) > (ImplicitGrapher::offset.z - 0.01f)
    ) {*/
    if (sphereClipsGraph) {
        if (dot(_, _) > (ImplicitGrapher::offset.x - 0.01f) * (ImplicitGrapher::offset.x - 0.01f)) {
            return -1.0f;
        }
    } else {
        if (dot(_, _) > (ImplicitGrapher::offset.x - 0.5f) * (ImplicitGrapher::offset.x - 0.5f)) {
            return -1.0f;
        }
    }

    _ *= sim.sphereRadiusPlusPointFive / ImplicitGrapher::defaultOffset.x;

    float px = _.x + sim.sphereRadiusPlusPointFive;
    float py = _.y + sim.sphereRadiusPlusPointFive;
    float pz = _.z + sim.sphereRadiusPlusPointFive;

    int pxi = floor(px * sim.pInvSpacing);
    int pyi = floor(py * sim.pInvSpacing);
    int pzi = floor(pz * sim.pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, sim.pNumX - 1);
    int y1 = min(pyi + 1, sim.pNumY - 1);
    int z1 = min(pzi + 1, sim.pNumZ - 1);

    float sum = 0.0f;

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * sim.pNumY * sim.pNumZ + yi * sim.pNumZ + zi;
                int first = sim.firstCellParticle[cellNr];
                int last = sim.firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = sim.cellParticleIds[j];
                    vec3 difference = sim.particles[id].position - _;
                    float distanceSquared = dot(difference, difference);
                    if(distanceSquared < sim.maxForceDistanceSquared) {
                        sum += 1.0f / distanceSquared;
                    }
                }
            }
        }
    }
    return sum - 2.0f;
}

NaiveSimulationFluidSurfaceView::NaiveSimulationFluidSurfaceView(const int &particleCount, const bool &fluidSurface, const int &graphSize, const float &sphereRadius, const bool &referenceFrameRotates, const bool &smoothSphereSurface) : View() {
    this->referenceFrameRotates = referenceFrameRotates;
    this->fluidSurface = fluidSurface;

    simulation.seed(particleCount, sphereRadius);

    if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
        environmentMapTextureTarget = GL_TEXTURE_2D;
        environmentMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
        environmentMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
        environmentMapTextureTarget = GL_TEXTURE_2D;
        environmentMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
        environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 2048, 2048, this);
    }else if(backgroundTexture == Texture::DefaultImages::RGB_CUBE){
        environmentMapTextureTarget = GL_TEXTURE_CUBE_MAP;
        environmentMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), CUBE_MAP_FRAGMENT_SHADER.c_str());
        environmentMap = CubeMap::createSimpleTextureCubemap();
    }
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(environmentMap.environmentTriangleVertices) / sizeof(PositionXYZ));

    if (fluidSurface) {
        graphNormalMapProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_NORMAL_MAP_FRAGMENT_SHADER.c_str());
        sphereNormalMapProgram = createVertexAndFragmentShaderProgram(SPHERE_VERTEX_SHADER.c_str(), SPHERE_NORMAL_MAP_FRAGMENT_SHADER.c_str());
        if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
            graphFluidSurfaceProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_SPHERE_MAP_FRAGMENT_SHADER.c_str());
            graphFluidSurfaceClipsSphereProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_CLIPS_SPHERE_SPHERE_MAP_FRAGMENT_SHADER.c_str());
            environmentMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
        }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
            graphFluidSurfaceProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_SPHERE_MAP_FRAGMENT_SHADER.c_str());
            graphFluidSurfaceClipsSphereProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_CLIPS_SPHERE_SPHERE_MAP_FRAGMENT_SHADER.c_str());
            environmentMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), SPHERE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
        }else if(backgroundTexture == Texture::DefaultImages::RGB_CUBE){
            graphFluidSurfaceProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_CUBE_MAP_FRAGMENT_SHADER.c_str());
            graphFluidSurfaceClipsSphereProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FLUID_SURFACE_CLIPS_SPHERE_CUBE_MAP_FRAGMENT_SHADER.c_str());
            environmentMapDoubleRefractionProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), CUBE_MAP_DOUBLE_REFRACTION_FRAGMENT_SHADER.c_str());
        }

        tilesVAO = VertexArrayObject(tilesVertices, sizeof(tilesVertices) / sizeof(PositionXYZ));

        implicitGrapher = ImplicitGrapher(ivec3(graphSize));

        sphere = Sphere(sphereRadius + 0.5f, 100);
        sphereVAO = VertexArrayObject(sphere);

        sphereClipsGraph = smoothSphereSurface;
    } else {
        cubeProgram = createVertexAndFragmentShaderProgram(_VERTEX_SHADER.c_str(), _FRAGMENT_SHADER.c_str());
        cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));
    }
}

NaiveSimulationFluidSurfaceView::~NaiveSimulationFluidSurfaceView(){
    glDeleteProgram(mProgram);
    delete simulation.particles;
}

void NaiveSimulationFluidSurfaceView::render(){
    Matrix4<float> translation, rotation, model, view, projection, mvp, cameraTransformation, inverseViewProjection;
    Matrix3<float> normalMatrix;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();

    if(fluidSurface) {

        enum Material {
            MERCURY,
            WATER,
            BUBBLE,
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
            case WATER:
                indexOfRefraction = 4.0f / 3.0f;
                reflectivity = -1.0f;
                twoSidedRefraction = YES;
                break;
            case BUBBLE:
                indexOfRefraction = 3.0f / 4.0f;
                reflectivity = 0.0f;
                twoSidedRefraction = NO;
                break;
        }

        float distanceToCenter = 100.0f * distanceToOrigin;

        translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -distanceToCenter));
        normalMatrix = referenceFrameRotates ? rotation.GetSubMatrix3().GetInverse() : normalMatrix.Identity();
        cameraTransformation = rotation.GetInverse() * translation * rotation * model.Translation(Vec3<float>(0.0f, 0.0f, 0.0f));

        ImplicitGrapher::calculateSurfaceOnCPU(fOfXYZFluidSurface, 0.1f * getFrameCount(), 10, ImplicitGrapher::defaultOffset, 3.0f / 7.0f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices, simulation);

        if (sphereClipsGraph) {
            float distanceToTangent = (pow(distanceToCenter, 2.0f) - pow(sphere.getRadius(), 2.0f)) / distanceToCenter;
            GLenum no_draw_buffer[1] = {GL_NONE};
            GLenum draw_buffer[1] = {GL_BACK};
            static Matrix4<float> inverseView;
            static Vec3<float> camPosition;
            static vec3 cameraPosition;
            model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
            view = referenceFrameRotates ? translation : translation * rotation;
            inverseView = (view * model).GetInverse();
            camPosition = (inverseView * Vec4<float>(0.0f, 0.0f, 0.0f, 1.0f)).XYZ();
            cameraPosition = vec3(camPosition.x, camPosition.y, camPosition.z);

            struct sortingUtility {
                static bool compareUvec3(const uvec3 &a, const uvec3 &b) {
                    vec3 positionA = (1.0f / 3.0f) * (ImplicitGrapher::vertices[a.v[0]].p + ImplicitGrapher::vertices[a.v[1]].p + ImplicitGrapher::vertices[a.v[2]].p);
                    vec3 positionB = (1.0f / 3.0f) * (ImplicitGrapher::vertices[b.v[0]].p + ImplicitGrapher::vertices[b.v[1]].p + ImplicitGrapher::vertices[b.v[2]].p);
                    vec3 differenceA = positionA - cameraPosition;
                    vec3 differenceB = positionB - cameraPosition;
                    float squaredDistanceA = dot(differenceA, differenceA);
                    float squaredDistanceB = dot(differenceB, differenceB);
                    return squaredDistanceA > squaredDistanceB;
                }
            };
            std::sort(ImplicitGrapher::indices, ImplicitGrapher::indices + ImplicitGrapher::numIndices / 3, sortingUtility::compareUvec3);

            // Render to texture
            glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
            glDrawBuffers(1, fbo.drawBuffers);
            glClearDepthf(1.0f);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            {
                // Render far frustum
                calculatePerspectiveSetViewport(maxViewAngle, distanceToTangent, zFar);
                {
                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                    view = referenceFrameRotates ? translation : translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render graph
                    glDepthFunc(GL_LEQUAL);
                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                    glUseProgram(graphNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(graphNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    glUniformMatrix3fv(glGetUniformLocation(graphNormalMapProgram, "normalMatrix"), 1, GL_FALSE, (GLfloat *) &normalMatrix);
                    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
                    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].p);
                    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].n);
                    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
                    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(0.0f));
                    view = translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render a sphere
                    glUseProgram(sphereNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(sphereNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    sphereVAO.drawArrays();

                    glCullFace(GL_BACK);
                }

                // Render near frustum
                calculatePerspectiveSetViewport(maxViewAngle, zNear, distanceToTangent);
                {
                    glClear(GL_DEPTH_BUFFER_BIT);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(0.0f));
                    view = translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render a sphere
                    glEnable(GL_CULL_FACE);
                    glDrawBuffers(1, no_draw_buffer);
                    glUseProgram(sphereNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(sphereNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    sphereVAO.drawArrays();

                    glDrawBuffers(1, fbo.drawBuffers);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                    view = referenceFrameRotates ? translation : translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render graph
                    glDepthMask(GL_FALSE);
                    glCullFace(GL_FRONT);
                    glDepthFunc(GL_GEQUAL);
                    glUseProgram(graphNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(graphNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    glUniformMatrix3fv(glGetUniformLocation(graphNormalMapProgram, "normalMatrix"), 1, GL_FALSE, (GLfloat *) &normalMatrix);
                    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
                    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].p);
                    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].n);
                    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
                    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

                    glDepthFunc(GL_LEQUAL);
                    glDepthMask(GL_TRUE);
                    glCullFace(GL_BACK);
                }
            }

            // Render to default frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearDepthf(0.0f);
            glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            {
                // Render far frustum
                calculatePerspectiveSetViewport(maxViewAngle, distanceToTangent, zFar);
                {
                    glEnable(GL_CULL_FACE);

                    if (!backgroundIsSolidColor) {
                        // Prepare model-view-projection matrix
                        model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                        view = referenceFrameRotates ? translation : translation * rotation;
                        projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                        mvp = projection * view * model;
                        cameraTransformation = rotation.GetInverse() * translation * rotation * model;

                        // Render environment map
                        glDisable(GL_DEPTH_TEST);
                        glUseProgram(environmentMapProgram);
                        glUniformMatrix4fv(glGetUniformLocation(environmentMapProgram, "inverseViewProjection"), 1, GL_FALSE, (GLfloat *) &inverseViewProjection);
                        glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
                        glActiveTexture(GL_TEXTURE0);
                        glUniform1i(glGetUniformLocation(environmentMapProgram, "environmentTexture"), 0);
                        environmentTriangleVAO.drawArrays();
                        glEnable(GL_DEPTH_TEST);
                    }

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(0.0f));
                    view = referenceFrameRotates ? translation : translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render a sphere
                    glCullFace(GL_FRONT);
                    glDepthFunc(GL_GEQUAL);
                    glDrawBuffers(1, no_draw_buffer);
                    glUseProgram(sphereNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(sphereNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    sphereVAO.drawArrays();
                    glDrawBuffers(1, draw_buffer);
                    glDepthFunc(GL_LEQUAL);
                    glCullFace(GL_BACK);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                    view = referenceFrameRotates ? translation : translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render graph
                    glDepthMask(GL_FALSE);
                    glUseProgram(graphFluidSurfaceClipsSphereProgram);
                    glUniformMatrix4fv(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    glUniformMatrix4fv(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "viewTransformation"), 1, GL_FALSE, (GLfloat *) &cameraTransformation);
                    glUniformMatrix3fv(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "normalMatrix"), 1, GL_FALSE, (GLfloat *) &normalMatrix);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
                    glUniform1i(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "environmentTexture"), 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
                    glUniform1i(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "image"), 1);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "reflectivity"), reflectivity);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "indexOfRefraction"), indexOfRefraction);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "inverseIOR"), 1.0f / indexOfRefraction);
                    glUniform1i(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "twoSidedRefraction"), twoSidedRefraction);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "screenWidth"), initialWidth);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "screenHeight"), initialHeight);
                    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
                    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].p);
                    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].n);
                    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
                    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

                    glDepthMask(GL_TRUE);
                }

                // Render near frustum
                calculatePerspectiveSetViewport(maxViewAngle, zNear, distanceToTangent);
                {
                    glClear(GL_DEPTH_BUFFER_BIT);
                    glEnable(GL_CULL_FACE);

                    // Prepare model-view-projection matrix
                    Matrix4<float> translation2, scale;
                    float bias = 0.001f;
                    translation2 = translation2.Translation(Vec3<float>(0.0f, 0.0f, bias - distanceToTangent));
                    float ratio = sqrt(powf(distanceToCenter, 2.0f) - powf(sphere.getRadius(), 2.0f)) / distanceToCenter;
                    scale = scale.Identity().Scale(Vec3<float>(ratio, ratio, 0.0f));
                    model = scale;
                    view = translation2;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Define stencil by rendering a sphere
                    glDepthFunc(GL_ALWAYS);
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    glUseProgram(sphereNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(sphereNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    sphereVAO.drawArrays();
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                    glDepthFunc(GL_LEQUAL);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                    view = referenceFrameRotates ? translation : translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render graph
                    glUseProgram(graphFluidSurfaceClipsSphereProgram);
                    glUniformMatrix4fv(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    glUniformMatrix4fv(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "viewTransformation"), 1, GL_FALSE, (GLfloat *) &cameraTransformation);
                    glUniformMatrix3fv(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "normalMatrix"), 1, GL_FALSE, (GLfloat *) &normalMatrix);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
                    glUniform1i(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "environmentTexture"), 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
                    glUniform1i(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "image"), 1);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "reflectivity"), reflectivity);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "indexOfRefraction"), indexOfRefraction);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "inverseIOR"), 1.0f / indexOfRefraction);
                    glUniform1i(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "twoSidedRefraction"), twoSidedRefraction);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "screenWidth"), initialWidth);
                    glUniform1f(glGetUniformLocation(graphFluidSurfaceClipsSphereProgram, "screenHeight"), initialHeight);
                    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
                    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].p);
                    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].n);
                    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
                    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(0.0f));
                    view = translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    // Render a sphere
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    glUseProgram(sphereNormalMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(sphereNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    sphereVAO.drawArrays();
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                    // Prepare model-view-projection matrix
                    model = model.Translation(Vec3<float>(0.0f));
                    view = translation * rotation;
                    projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                    mvp = projection * view * model;

                    cameraTransformation = rotation.GetInverse() * view;

                    // Render a sphere
                    glDepthFunc(GL_GREATER);
                    glUseProgram(environmentMapDoubleRefractionProgram);
                    glUniformMatrix4fv(glGetUniformLocation(environmentMapDoubleRefractionProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                    glUniformMatrix4fv(glGetUniformLocation(environmentMapDoubleRefractionProgram, "viewTransformation"), 1, GL_FALSE, (GLfloat *) &cameraTransformation);
                    glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
                    glActiveTexture(GL_TEXTURE0);
                    glUniform1i(glGetUniformLocation(environmentMapDoubleRefractionProgram, "environmentTexture"), 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
                    glUniform1i(glGetUniformLocation(environmentMapDoubleRefractionProgram, "image"), 1);
                    glUniform1f(glGetUniformLocation(environmentMapDoubleRefractionProgram, "reflectivity"), reflectivity);
                    glUniform1f(glGetUniformLocation(environmentMapDoubleRefractionProgram, "indexOfRefraction"), indexOfRefraction);
                    glUniform1f(glGetUniformLocation(environmentMapDoubleRefractionProgram, "inverseIOR"), 1.0f / indexOfRefraction);
                    glUniform1i(glGetUniformLocation(environmentMapDoubleRefractionProgram, "twoSidedRefraction"), twoSidedRefraction);
                    glUniform1f(glGetUniformLocation(environmentMapDoubleRefractionProgram, "screenWidth"), initialWidth);
                    glUniform1f(glGetUniformLocation(environmentMapDoubleRefractionProgram, "screenHeight"), initialHeight);
                    sphereVAO.drawArrays();
                    glDepthFunc(GL_LEQUAL);
                }
            }

        } else {

            // Render to texture
            glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
            glDrawBuffers(1, fbo.drawBuffers);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            {
                // Prepare model-view-projection matrix
                model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                view = referenceFrameRotates ? translation : translation * rotation;
                projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                mvp = projection * view * model;

                // Render graph
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
                glUseProgram(graphNormalMapProgram);
                glUniformMatrix4fv(glGetUniformLocation(graphNormalMapProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                glUniformMatrix3fv(glGetUniformLocation(graphNormalMapProgram, "normalMatrix"), 1, GL_FALSE, (GLfloat *) &normalMatrix);
                glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
                glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].p);
                glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].n);
                glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
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
                model = model.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
                view = referenceFrameRotates ? translation : translation * rotation;
                projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
                mvp = projection * view * model;
                if(referenceFrameRotates){
                    cameraTransformation = rotation.GetInverse() * translation * model;
                }else {
                    cameraTransformation = rotation.GetInverse() * translation * rotation * model;
                }

                // Render graph
                glUseProgram(graphFluidSurfaceProgram);
                glUniformMatrix4fv(glGetUniformLocation(graphFluidSurfaceProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
                glUniformMatrix4fv(glGetUniformLocation(graphFluidSurfaceProgram, "viewTransformation"), 1, GL_FALSE, (GLfloat *) &cameraTransformation);
                glUniformMatrix3fv(glGetUniformLocation(graphFluidSurfaceProgram, "normalMatrix"), 1, GL_FALSE, (GLfloat *) &normalMatrix);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
                glUniform1i(glGetUniformLocation(graphFluidSurfaceProgram, "environmentTexture"), 0);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
                glUniform1i(glGetUniformLocation(graphFluidSurfaceProgram, "image"), 1);
                glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "reflectivity"), reflectivity);
                glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "indexOfRefraction"), indexOfRefraction);
                glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "inverseIOR"), 1.0f / indexOfRefraction);
                glUniform1i(glGetUniformLocation(graphFluidSurfaceProgram, "twoSidedRefraction"), twoSidedRefraction);
                glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "screenWidth"), initialWidth);
                glUniform1f(glGetUniformLocation(graphFluidSurfaceProgram, "screenHeight"), initialHeight);
                glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
                glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].p);
                glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid *) &ImplicitGrapher::vertices[0].n);
                glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
                glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
                glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                if(!backgroundIsSolidColor) {
                    // Render sphere map
                    glUseProgram(environmentMapProgram);
                    glUniformMatrix4fv(glGetUniformLocation(environmentMapProgram, "inverseViewProjection"), 1, GL_FALSE, (GLfloat *) &inverseViewProjection);
                    glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
                    glActiveTexture(GL_TEXTURE1);
                    glUniform1i(glGetUniformLocation(environmentMapProgram, "environmentTexture"), 1);
                    environmentTriangleVAO.drawArrays();
                }
            }
        }
    } else {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(cubeProgram);
        projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
        translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 50.0f * (distanceToOrigin - 1.0f)));
        rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
        for (int i = 0; i < simulation.particleCount; i++) {
            model = model.Translation(Vec3<float>(simulation.particles[i].position.x, simulation.particles[i].position.y, simulation.particles[i].position.z));
            view = referenceFrameRotates ? translation : translation * rotation;
            mvp = projection * view * model;
            glUniformMatrix4fv(glGetUniformLocation(cubeProgram, "mvp"), 1, GL_FALSE, (GLfloat *) &mvp);
            vec4 color = vec4(
                    0.06125f * simulation.particles[i].velocity.x + 0.5f,
                    -0.06125f * simulation.particles[i].velocity.y + 0.5f,
                    -0.06125f * simulation.particles[i].velocity.z + 0.5f,
                    1.0f
            );
            glUniform4fv(glGetUniformLocation(cubeProgram, "color"), 1, color.v);
            cubeVAO.drawArrays();
        }

        if(!backgroundIsSolidColor) {
            // Render sphere map
            glUseProgram(environmentMapProgram);
            glUniformMatrix4fv(glGetUniformLocation(environmentMapProgram, "inverseViewProjection"), 1, GL_FALSE, (GLfloat *) &inverseViewProjection);
            glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(environmentMapProgram, "environmentTexture"), 0);
            environmentTriangleVAO.drawArrays();
        }

    }

    // Simulate
    vec3 forceVector = computeForce(gravity, referenceFrameRotates, rotation);
    for(int i = 0; i < 5; i++){
        simulation.simulate(forceVector, efficiency);
    }
}
