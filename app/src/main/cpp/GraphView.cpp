//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "GraphView.h"

using std::to_string;

GraphView::GraphView() : View() {

}

GraphView::GraphView(const string& equation) : View() {
    graphProgram = createVertexAndFragmentShaderProgram(GRAPH_VERTEX_SHADER.c_str(), GRAPH_FRAGMENT_SHADER.c_str());
    cubeProgram = createVertexAndFragmentShaderProgram(CUBE_VERTEX_SHADER.c_str(), CUBE_FRAGMENT_SHADER.c_str());

    implicitGrapher = ImplicitGrapher(ivec3(29));
    if(equation == "") {
        //ImplicitGrapher::surfaceEquation = 40; // Resets to 0 on the first render
        //for (int i = 0; i < ImplicitGrapher::numOfDefaultEquations; i++) {
        //    ImplicitGrapher::memoryEquations[i][1] = ImplicitGrapher::defaultEquations[i][1];
        //    //Convert any 'π' symbols to a ''
        //    ImplicitGrapher::convertPiSymbol(ImplicitGrapher::memoryEquations[i][1]);
        //    ImplicitGrapher::processEquation(i);
        //    ImplicitGrapher::numOfEquationsInMemory++;
        //}
    }else{
        ImplicitGrapher::surfaceEquation = 0;
        ImplicitGrapher::memoryEquations[ImplicitGrapher::surfaceEquation][1] = equation;
        ImplicitGrapher::processEquation(ImplicitGrapher::surfaceEquation);
        ImplicitGrapher::numOfEquationsInMemory++;
    }

    simulation.initialize(Computation::ComputationOptions::CPU);
    //simulation.initialize(Computation::ComputationOptions::GPU);
    simulation.computeShader.gIndexBufferBinding = SimpleNBodySimulation::DEFAULT_INDEX_BUFFER_BINDING;

    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

    sphereMapProgram = createVertexAndFragmentShaderProgram(SPHERE_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
    if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
        sphereMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
    }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
        sphereMap = SphereMap(Texture::DefaultImages::MANDELBROT, 2048, 2048, this);
    }
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(sphereMap.environmentTriangleVertices) / sizeof(PositionXYZ));
}

GraphView::~GraphView(){
    glDeleteProgram(mProgram);
}

void GraphView::render(){
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    ImplicitGrapher::calculateSurfaceOnCPU(ImplicitGrapher::fOfXYZ, 0.1f * getFrameCount(), 10, vec3(0.0f), 0.15f, false, false, ImplicitGrapher::vertices, ImplicitGrapher::indices, ImplicitGrapher::numIndices);

    // Prepare model-view-projection matrix
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -120.0f * distanceToOrigin));
    Matrix4<float> rotation;
    rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> mvp = orientationAdjustedPerspective * translation * rotation;

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

    if(!backgroundIsSolidColor) {
        Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();
        // Render sphere map
        glUseProgram(sphereMapProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(sphereMapProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat *) &inverseViewProjection);
        glBindTexture(GL_TEXTURE_2D, sphereMap.getTextureId());
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(sphereMapProgram, "environmentTexture"), 0);
        environmentTriangleVAO.drawArrays();
    }

    //if(getFrameCount() % 100 == 0){
    //    ImplicitGrapher::surfaceEquation++;
    //    if(ImplicitGrapher::surfaceEquation == ImplicitGrapher::numOfEquationsInMemory){
    //        ImplicitGrapher::surfaceEquation = 0;
    //    }
    //}

    // Render cubes
    /*if(simulation.getComputationOption() == Computation::ComputationOptions::CPU) {
        simulation.pushData2GPU();
    }
    glUseProgram(cubeProgram);
    glUniformMatrix4fv(
            glGetUniformLocation(cubeProgram, "mvp"),
            1,
            GL_FALSE,
            (GLfloat*)&mvp);
    glBindVertexArray(cubeVAO.getArrayObjectId());
    glBindBuffer(GL_ARRAY_BUFFER, simulation.computeShader.gVBO);
    glEnableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glVertexAttribDivisor(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 1);
    glVertexAttribDivisor(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 1);
    for(int i = 0; i < SimpleNBodySimulation::PARTICLES_PER_CHUNK; i++){
        glVertexAttribPointer(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleNBodySimulation::cacheChunk), (const GLvoid*)(offsetof(Simulation::Particle, position) + sizeof(Simulation::Particle) * i));
        glVertexAttribPointer(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(SimpleNBodySimulation::cacheChunk), (const GLvoid*)(offsetof(Simulation::Particle, velocity) + sizeof(Simulation::Particle) * i));
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, cubeVAO.getNumVertices(), SimpleNBodySimulation::NUM_CACHE_CHUNKS);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(SimpleNBodySimulation::OFFSET_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(SimpleNBodySimulation::VELOCITY_ATTRIBUTE_LOCATION);
    glBindVertexArray(0);

    simulation.simulate(1, NO, NO);*/
}
