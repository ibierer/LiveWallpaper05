//
// Created by Immanuel Bierer on 1/15/2024.
//

#include "NaiveSimulationFluidSurfaceView.h"

using std::min;
using std::max;

NaiveSimulation* sim;

float fOfXYZFluidSurface(vec3 _) {
    _ -= ImplicitGrapher::offset;

    if (
            abs(_.x) > (ImplicitGrapher::offset.x - 0.01f) ||
            abs(_.y) > (ImplicitGrapher::offset.y - 0.01f) ||
            abs(_.z) > (ImplicitGrapher::offset.z - 0.01f)
    ) {
    //if(dot(_, _) > 6.99f * 6.99f) {
        return -1.0f;
    }

    _ *= sim->sphereRadiusPlusPointFive / ImplicitGrapher::defaultOffset.x;

    float px = _.x + sim->sphereRadiusPlusPointFive;
    float py = _.y + sim->sphereRadiusPlusPointFive;
    float pz = _.z + sim->sphereRadiusPlusPointFive;

    int pxi = floor(px * sim->pInvSpacing);
    int pyi = floor(py * sim->pInvSpacing);
    int pzi = floor(pz * sim->pInvSpacing);
    int x0 = max(pxi - 1, 0);
    int y0 = max(pyi - 1, 0);
    int z0 = max(pzi - 1, 0);
    int x1 = min(pxi + 1, sim->pNumX - 1);
    int y1 = min(pyi + 1, sim->pNumY - 1);
    int z1 = min(pzi + 1, sim->pNumZ - 1);

    float sum = 0.0f;

    for (int xi = x0; xi <= x1; xi++) {
        for (int yi = y0; yi <= y1; yi++) {
            for (int zi = z0; zi <= z1; zi++) {
                int cellNr = xi * sim->pNumY * sim->pNumZ + yi * sim->pNumZ + zi;
                int first = sim->firstCellParticle[cellNr];
                int last = sim->firstCellParticle[cellNr + 1];
                for (int j = first; j < last; j++) {
                    int id = sim->cellParticleIds[j];
                    vec3 difference = sim->particles[id].position - _;
                    float distanceSquared = dot(difference, difference);
                    if(distanceSquared < sim->maxForceDistanceSquared) {
                        sum += 1.0f / distanceSquared;
                    }
                }
            }
        }
    }
    return sum - 2.0f;
}

NaiveSimulationFluidSurfaceView::NaiveSimulationFluidSurfaceView(const int &particleCount, const int &graphSize, const float &sphereRadius) : View() {
    mProgram = createVertexAndFragmentShaderProgram(TILES_VERTEX_SHADER.c_str(), TILES_FRAGMENT_SHADER.c_str());
    tilesVAO = VertexArrayObject(tilesVertices, sizeof(tilesVertices) / sizeof(PositionXYZ));
    texture = Texture(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    //texture = Texture::staticallyGenerateMandelbrotWithVertexShader(Texture(GL_RGB, 16384, 16384, 0, GL_LINEAR), this);
    fbo = FBO(
            (void*) new Texture(GL_RGB, texture.getWidth(), texture.getHeight(), 0, GL_LINEAR),
            YES,
            NO);


    cubeProgram = createVertexAndFragmentShaderProgram(CUBE_VERTEX_SHADER.c_str(), CUBE_FRAGMENT_SHADER.c_str());
    graphProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FRAGMENT_SHADER.c_str());
    sphereProgram = createVertexAndFragmentShaderProgram(SPHERE_VERTEX_SHADER.c_str(), SPHERE_FRAGMENT_SHADER.c_str());
    sphereMapProgram = createVertexAndFragmentShaderProgram(SPHERE_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());

    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

    implicitGrapher = ImplicitGrapher(ivec3(graphSize));

    simulation.seed(particleCount, sphereRadius);
    sphereVAO = VertexArrayObject(Sphere(sphereRadius, 100));

    sphereMap = SphereMap(Texture::DefaultImages::MANDELBROT, 16384, 16384, this);
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(sphereMap.environmentTriangleVertices) / sizeof(PositionXYZ));

    sim = &simulation;
}

NaiveSimulationFluidSurfaceView::~NaiveSimulationFluidSurfaceView(){
    glDeleteProgram(mProgram);
    delete simulation.particles;
}

void NaiveSimulationFluidSurfaceView::render(){

    ImplicitGrapher::calculateSurfaceOnCPU(fOfXYZFluidSurface, 0.1f * getFrameCount(), 10, ImplicitGrapher::defaultOffset, 3.0f / 7.0f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    {
        glDisable(GL_CULL_FACE);
        int storeWidth = width;
        width = fbo.getWidth();
        int storeHeight = height;
        height = fbo.getHeight();
        calculatePerspectiveSetViewport(60.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo.getFrameBuffer());
        glDrawBuffers(1, fbo.drawBuffers);
        glClearColor(backgroundColor.r + 0.5f, backgroundColor.g + 0.5f, backgroundColor.b + 0.5f, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);

        // Draw graph
        {
            Matrix4<float> translation;
            translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 50.0f * (zoom - 1.0f)));
            Matrix4<float> translation2;
            Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));

            // Prepare model-view-projection matrix
            translation2 = translation2.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
            Matrix4<float> mvp;
            if(referenceFrameRotates){
                mvp = perspective * translation * translation2;
            }else{
                mvp = orientationAdjustedPerspective * translation * rotation * translation2;
            }

            // Render graph
            glUseProgram(graphProgram);
            glUniformMatrix4fv(
                    glGetUniformLocation(graphProgram, "mvp"),
                    1,
                    GL_FALSE,
                    (GLfloat*)&mvp);
            glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
            glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
            glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].p);
            glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].n);
            glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
            glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
            glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
        }

        Matrix4<float> translation;
        translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 50.0f * (zoom - 1.0f)));
        Matrix4<float> translation2;
        translation2 = translation2.Translation(Vec3<float>(-0.5f));
        Matrix4<float> rotation;
        rotation = Matrix4<float>(quaternionTo3x3(
                Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z,
                            rotationVector.w)));
        Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation * translation2;

        glUseProgram(mProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.getTextureId());
        glUniform1i(glGetUniformLocation(mProgram, "image"), 0);

        tilesVAO.drawArrays();

        width = storeWidth;
        height = storeHeight;
        calculatePerspectiveSetViewport(60.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mvp = orientationAdjustedPerspective * translation * rotation * translation2;

        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fbo.getRenderedTextureId());
        glUniform1i(glGetUniformLocation(mProgram, "image"), 1);

        tilesVAO.drawArrays();
    }








    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(cubeProgram);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, 50.0f * (zoom - 1.0f)));
    Matrix4<float> translation2;
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    for(int i = 0; i < simulation.particleCount; i++) {
        translation2 = translation2.Translation(Vec3<float>(simulation.particles[i].position.x, simulation.particles[i].position.y, simulation.particles[i].position.z));
        Matrix4<float> mvp;
        if(referenceFrameRotates) {
            mvp = perspective * translation * translation2;
        }else{
            mvp = orientationAdjustedPerspective * translation * rotation * translation2;
        }
        glUniformMatrix4fv(
                glGetUniformLocation(cubeProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        vec4 color = vec4(
                0.06125f * simulation.particles[i].velocity.x + 0.5f,
                -0.06125f * simulation.particles[i].velocity.y + 0.5f,
                -0.06125f * simulation.particles[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(cubeProgram, "color"), 1, color.v);
        cubeVAO.drawArrays();
    }

    // Prepare model-view-projection matrix
    translation2 = translation2.Translation(Vec3<float>(ImplicitGrapher::defaultOffset.x, ImplicitGrapher::defaultOffset.y, ImplicitGrapher::defaultOffset.z));
    Matrix4<float> mvp;
    if(referenceFrameRotates){
        mvp = perspective * translation * translation2;
    }else{
        mvp = orientationAdjustedPerspective * translation * rotation * translation2;
    }

    // Render graph
    glUseProgram(graphProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(graphProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);
    glVertexAttribPointer(POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].p);
    glVertexAttribPointer(NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(PositionXYZNormalXYZ), (const GLvoid*)&ImplicitGrapher::vertices[0].n);
    glDrawElements(GL_TRIANGLES, ImplicitGrapher::numIndices, GL_UNSIGNED_INT, ImplicitGrapher::indices);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE_LOCATION);

    // Render a sphere
    glCullFace(GL_FRONT);
    mvp = perspective * translation;
    glUseProgram(sphereProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    sphereVAO.drawArrays();
    glCullFace(GL_BACK);

    // Render sphere map
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();
    glUseProgram(sphereMapProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(sphereMapProgram, "inverseViewProjection"),
            1,
            GL_FALSE,
            (GLfloat*)&inverseViewProjection);
    glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(sphereMapProgram, "environmentTexture"), 1);
    environmentTriangleVAO.drawArrays();

    for(int i = 0; i < 5; i++){
        if(referenceFrameRotates){
            simulation.simulate(compensateForOrientation(accelerometerVector));
        }else {
            simulation.simulate(quaternionTo3x3(rotationVector) * (-accelerometerVector));
        }
    }

    checkGlError("Renderer::render");
}
