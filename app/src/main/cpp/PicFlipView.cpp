//
// Created by Immanuel Bierer on 12/4/2023.
//

#include "PicFlipView.h"

PicFlipView::PicFlipView(const bool &referenceFrameRotates) : View() {
    this->referenceFrameRotates = referenceFrameRotates;
    this->gravity = gravity;

    mProgram = createVertexAndFragmentShaderProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
    cubeVAO = VertexArrayObject(Cube(1.0f, Cube::ColorOption::SOLID));

    if(backgroundTexture == Texture::DefaultImages::MS_PAINT_COLORS){
        environmentMapTextureTarget = GL_TEXTURE_2D;
        environmentMap = SphereMap(Texture::DefaultImages::MS_PAINT_COLORS, 1536, 1536, this);
        environmentMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
    }else if(backgroundTexture == Texture::DefaultImages::MANDELBROT){
        environmentMapTextureTarget = GL_TEXTURE_2D;
        environmentMap = SphereMap(Texture::DefaultImages::MANDELBROT, 2048, 2048, this);
        environmentMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), SPHERE_MAP_FRAGMENT_SHADER.c_str());
    }else if(backgroundTexture == Texture::DefaultImages::RGB_CUBE){
        environmentMapTextureTarget = GL_TEXTURE_CUBE_MAP;
        environmentMap = CubeMap::createSimpleTextureCubemap();
        environmentMapProgram = createVertexAndFragmentShaderProgram(ENVIRONMENT_MAP_VERTEX_SHADER.c_str(), CUBE_MAP_FRAGMENT_SHADER.c_str());
    }
    environmentTriangleVAO = VertexArrayObject(EnvironmentMap::environmentTriangleVertices, sizeof(environmentMap.environmentTriangleVertices) / sizeof(PositionXYZ));

    // create fluid

    fluid = new FlipFluid();
}

PicFlipView::~PicFlipView(){

}

void PicFlipView::simulate(const vec3 &acceleration) {
    fluid->simulate(acceleration);
    fluid->frameNr++;
}

void PicFlipView::render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_LOCATION);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mProgram);
    Matrix4<float> translation;
    translation = translation.Translation(Vec3<float>(0.0f, 0.0f, -100.0f * distanceToOrigin));
    Matrix4<float> translation2;
    Matrix4<float> rotation = Matrix4<float>(quaternionTo3x3(Vec4<float>(rotationVector.x, rotationVector.y, rotationVector.z, rotationVector.w)));
    Matrix4<float> view = referenceFrameRotates ? translation : translation * rotation;
    Matrix4<float> projection = referenceFrameRotates ? perspective : orientationAdjustedPerspective;
    Matrix4<float> inverseViewProjection = (orientationAdjustedPerspective * rotation).GetInverse();
    for (int i = 0; i < fluid->numParticles; i++) {
        translation2 = translation2.Translation(10.0f * Vec3<float>(fluid->data->particles[i].position.x, fluid->data->particles[i].position.y, fluid->data->particles[i].position.z) - Vec3<float>(15.0f));
        Matrix4<float> model = translation2;
        Matrix4<float> mvp = projection * view * model;
        glUniformMatrix4fv(
                glGetUniformLocation(mProgram, "mvp"),
                1,
                GL_FALSE,
                (GLfloat *) &mvp);
        vec4 color = vec4(
                0.5f * fluid->data->particles[i].velocity.x + 0.5f,
                -0.5f * fluid->data->particles[i].velocity.y + 0.5f,
                -0.5f * fluid->data->particles[i].velocity.z + 0.5f,
                1.0f
        );
        glUniform4fv(glGetUniformLocation(mProgram, "color"), 1, color.v);
        cubeVAO.drawArrays();
    }

    if(!backgroundIsSolidColor) {
        // Render environment map
        glUseProgram(environmentMapProgram);
        glUniformMatrix4fv(
                glGetUniformLocation(environmentMapProgram, "inverseViewProjection"),
                1,
                GL_FALSE,
                (GLfloat *) &inverseViewProjection);
        glBindTexture(environmentMapTextureTarget, environmentMap.getTextureId());
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(environmentMapProgram, "environmentTexture"), 0);
        environmentTriangleVAO.drawArrays();
    }

    // Simulate
    vec3 forceVector = computeForce(gravity, referenceFrameRotates, rotation);
    for(int i = 0; i < 2; i++){
        simulate(forceVector);
    }
}
