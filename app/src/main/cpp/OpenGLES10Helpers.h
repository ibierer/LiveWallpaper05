#ifndef OPENGLES10HELPERS
#define OPENGLES10HELPERS

#include <stack>
#include "vectors.h"
//#include "Shader.h"

using cy::Matrix4;
using cy::Vec3;

/*GLfloat box[] = {
        // FRONT
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // BACK
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        // LEFT
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        // RIGHT
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        // TOP
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        // BOTTOM
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,
};

Shader basic;

char vertexShader[] =
        "#version 320 es\n"
        "layout(location = " STRV(POSITION_ATTRIBUTE_LOCATION) ") in vec3 pos;\n"
"uniform mat4 u_mvpMatrix;\n"
"void main() {\n"
"    gl_Position = u_mvpMatrix*vec4(pos, 1.0);\n"
"}\n"
;

char fragmentShader[] =
        "#version 320 es\n"
        "precision mediump float;\n"
        "uniform vec4 color;\n"
        "layout(location = 0) out vec4 outColor;             \n"
        "void main() {\n"
        "    outColor = color;\n"
        "}\n"
;

void placeholder(GLuint program){
    glUseProgram(program);
}*/

class {
public:
    /*void glTranslatef(float x, float y, float z){
        esTranslate(&transformation, x, y, z);
        passMVPtoGPU();
    }
    void glColor4f(float r, float g, float b, float a){
        glUniform4f(glGetUniformLocation(basic.mProgram, "color"), r, g, b, a);
    }
    void glPushMatrix(){
        transformationStack.push(transformation);
    }
    void glPopMatrix(){
        if(!transformationStack.empty()){
            ESMatrix* top = &transformationStack.top();
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    transformation.m[i][j] = top->m[i][j];
                }
            }
            transformationStack.pop();
        }
        passMVPtoGPU();
    }

    void glUseProgram(GLuint program){
        placeholder(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvpMatrix"), 1, GL_FALSE, (GLfloat*)&mvpMatrix);
    }*/

    Matrix4<float> rotationVectorTo4by4Matrix(const vec4& rotationVector) {
        mat3 matrix = quaternionTo3x3(rotationVector);
        Matrix4<float> _return;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                _return.cell[4 * i + j] = matrix.m[i][j];
            }
            _return.cell[4 * i + 3] = 0.0f;
            _return.cell[4 * 3 + i] = 0.0f;
        }
        _return.cell[4 * 3 + 3] = 1.0f;
        return _return;
    }

    void rotateTransformationMatrix(const vec4& rotationVectorInput) {
        Matrix4<float> rotationVector4by4Matrix = rotationVectorTo4by4Matrix(rotationVectorInput);
        Matrix4<float> copyOfTransformation = transformation;
        //esMatrixMultiply(&transformation, &rotationVector4by4Matrix, &copyOfTransformation);
        transformation = rotationVector4by4Matrix * copyOfTransformation;
    }

    /*void drawCube(float x, float y, float z){
        glVertexPointer(3, GL_FLOAT, 0, box);

        glPushMatrix();

        glTranslatef(x, y, z);

        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        glPopMatrix();
    }
    void drawCube(vec3 position){
        drawCube(position.x, position.y, position.z);
    }
    void drawCubeColored(float x, float y, float z, vec4 color){
        glVertexPointer(3, GL_FLOAT, 0, box);

        glPushMatrix();

        glTranslatef(x, y, z);

        glColor4f(color.r, color.g, color.b, color.a);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

        glPopMatrix();
    }
    void drawCubeColored(vec3 position, vec4 color){
        drawCubeColored(position.x, position.y, position.z, color);
    }
    bool initializeShaders(){
        basic = Shader(vertexShader, fragmentShader);
        shadersInitialized = true;
        return true;
    }
    ESMatrix getMVP(){
        return mvpMatrix;
    }
private:
    bool shadersInitialized = false;*/
    Matrix4<float> transformation;
    /*ESMatrix projection;
    ESMatrix mvpMatrix;
    std::stack <ESMatrix> transformationStack;
    void passMVPtoGPU(){
        esMatrixMultiply(&mvpMatrix, &transformation, &projection);
        basic.useProgram(mvpMatrix);
    }*/
} gl;

#endif