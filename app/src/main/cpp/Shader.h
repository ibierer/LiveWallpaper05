#ifndef SHADER_H
#define SHADER_H

#include <map>
#include "vectors.h"
#include "esUtil.h"

using std::string;
using std::map;
using std::pair;

#define STR(s) #s
#define STRV(s) STR(s)
#define POSITION_ATTRIBUTE_LOCATION 0
#define NORMAL_ATTRIBUTE_LOCATION 1
#define UV_ATTRIBUTE_LOCATION 2

class Shader {
private:
	map<string, GLint> unsignedIntUniformLocations;
	map<string, GLint> intUniformLocations;
	map<string, GLint> floatUniformLocations;
	map<GLint, unsigned int*> unsignedIntPointers;
	map<GLint, int*> intPointers;
	map<GLint, float*> floatPointers;
	map<GLint, unsigned int> unsignedInts;
	map<GLint, int> ints;
	map<GLint, float> floats;
public:
	void setUniforms() {
		for (map<string, GLint>::iterator i = unsignedIntUniformLocations.begin(); i != unsignedIntUniformLocations.end(); i++) {
			glUniform1ui(i->second, *unsignedIntPointers[i->second]);
		}
		for (map<string, GLint>::iterator i = intUniformLocations.begin(); i != intUniformLocations.end(); i++) {
			glUniform1i(i->second, *intPointers[i->second]);
		}
		for (map<string, GLint>::iterator i = floatUniformLocations.begin(); i != floatUniformLocations.end(); i++) {
			glUniform1f(i->second, *floatPointers[i->second]);
		}
	}
	GLuint mProgram;
	GLuint mvpMatrixLocation;
	GLuint cameraPositionLocation;
	Shader() {

	}
	Shader(char vertexShaderString[], char fragmentShaderString[]) {
		mProgram = esLoadProgram(vertexShaderString, fragmentShaderString);
		mvpMatrixLocation = glGetUniformLocation(mProgram, "u_mvpMatrix");
		cameraPositionLocation = glGetUniformLocation(mProgram, "cameraPosition");
	}
	void useProgram(ESMatrix mvpMatrix) {
		glUseProgram(mProgram);
		glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, (GLfloat*)&mvpMatrix);
		setUniforms();
	}
	void useProgram(ESMatrix mvpMatrix, vec3 cameraPosition) {
		glUseProgram(mProgram);
		glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, (GLfloat*)&mvpMatrix);
		glUniform3f(cameraPositionLocation, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		setUniforms();
	}
	void setUniform(const char* name, unsigned int value) {
		glUniform1ui(glGetUniformLocation(mProgram, name), value);
	}
	void setUniform(const char* name, int value) {
		glUniform1i(glGetUniformLocation(mProgram, name), value);
	}
	void setUniform(const char* name, float value) {
		glUniform1f(glGetUniformLocation(mProgram, name), value);
	}
	void setUniform(const char* name, ESMatrix mvpMatrix) {
		glUniformMatrix4fv(mvpMatrixLocation, 1, GL_FALSE, (GLfloat*)&mvpMatrix);
	}
	void setUniform(const char* name, vec2 value) {
		glUniform2f(glGetUniformLocation(mProgram, name), value.x, value.y);
	}
	void setUniform(const char* name, ivec2 value) {
		glUniform2i(glGetUniformLocation(mProgram, name), value.x, value.y);
	}
	void setUniform(const char* name, vec4 value) {
		glUniform4f(glGetUniformLocation(mProgram, name), value.x, value.y, value.z, value.w);
	}
	void mapUniformToVariable(const char* name, unsigned int* pointer) {
		GLint uniformLocation = glGetUniformLocation(mProgram, name);
		unsignedIntUniformLocations.insert(pair<string, GLint>(name, uniformLocation));
		unsignedIntPointers.insert(pair<GLint, unsigned int*>(uniformLocation, pointer));
	}
	void mapUniformToVariable(const char* name, int* pointer) {
		GLint uniformLocation = glGetUniformLocation(mProgram, name);
		intUniformLocations.insert(pair<string, GLint>(name, uniformLocation));
		intPointers.insert(pair<GLint, int*>(uniformLocation, pointer));
	}
	void mapUniformToVariable(const char* name, float* pointer) {
		GLint uniformLocation = glGetUniformLocation(mProgram, name);
		floatUniformLocations.insert(pair<string, GLint>(name, uniformLocation));
		floatPointers.insert(pair<GLint, float*>(uniformLocation, pointer));
	}
};

#endif