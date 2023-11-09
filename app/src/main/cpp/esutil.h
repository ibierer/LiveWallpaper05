#ifndef ESUTIL_H
#define ESUTIL_H

#include <GLES3/gl31.h>
#include <math.h>
#include "logger.h"

#define ESUTIL_API
#define ESCALLBACK

typedef struct {
	GLfloat m[4][4];
} ESMatrix;

typedef struct ESContext ESContext;
#define ESUTIL_API
#define ESCALLBACK

struct ESContext {
	/// Put platform specific data here
	//void* platformData;

	/// Put your user data here...
	//* userData;

	/// Window width
	//GLint       width;

	/// Window height
	//GLint       height;

#ifndef __APPLE__
	/// Display handle
	//EGLNativeDisplayType eglNativeDisplay;

	/// Window handle
	//EGLNativeWindowType  eglNativeWindow;

	/// EGL display
	//EGLDisplay  eglDisplay;

	/// EGL context
	//EGLContext  eglContext;

	/// EGL surface
	//EGLSurface  eglSurface;
#endif

	/// Callbacks
	void (ESCALLBACK* drawFunc) (ESContext*);
	void (ESCALLBACK* shutdownFunc) (ESContext*);
	void (ESCALLBACK* keyFunc) (ESContext*, unsigned char, int, int);
	void (ESCALLBACK* updateFunc) (ESContext*, float deltaTime);
};

GLboolean ESUTIL_API esCreateWindow(ESContext* esContext, const char* title, GLint width, GLint height, GLuint flags);
void ESUTIL_API esRegisterDrawFunc(ESContext* esContext, void (ESCALLBACK* drawFunc) (ESContext*));
void ESUTIL_API esRegisterShutdownFunc(ESContext* esContext, void (ESCALLBACK* shutdownFunc) (ESContext*));
void ESUTIL_API esRegisterUpdateFunc(ESContext* esContext, void (ESCALLBACK* updateFunc) (ESContext*, float));
void ESUTIL_API esRegisterKeyFunc(ESContext* esContext, void (ESCALLBACK* drawFunc) (ESContext*, unsigned char, int, int));
void ESUTIL_API esLogMessage(const char* formatStr, ...);
GLuint ESUTIL_API esLoadShader(GLenum type, const char* shaderSrc);
GLuint ESUTIL_API esLoadProgram(const char* vertShaderSrc, const char* fragShaderSrc);
int ESUTIL_API esGenSphere(int numSlices, float radius, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices);
int ESUTIL_API esGenCube(float scale, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices);
int ESUTIL_API esGenSquareGrid(int size, GLfloat** vertices, GLuint** indices);
char* ESUTIL_API esLoadTGA(void* ioContext, const char* fileName, int* width, int* height);
void ESUTIL_API esScale(ESMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz);
void ESUTIL_API esTranslate(ESMatrix* result, GLfloat tx, GLfloat ty, GLfloat tz);
void ESUTIL_API esRotate(ESMatrix* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void ESUTIL_API esFrustum(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ);
void ESUTIL_API esPerspective(ESMatrix* result, float fovy, float aspect, float nearZ, float farZ);
void ESUTIL_API esOrtho(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ);
void ESUTIL_API esMatrixMultiply(ESMatrix* result, ESMatrix* srcA, ESMatrix* srcB);
void ESUTIL_API esMatrixLoadIdentity(ESMatrix* result);
void ESUTIL_API esMatrixLookAt(ESMatrix* result, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ);
///
//  Public Functions
//

//
/// \brief Create a window with the specified parameters
/// \param esContext Application context
/// \param title Name for title bar of window
/// \param width Width in pixels of window to create
/// \param height Height in pixels of window to create
/// \param flags Bitfield for the window creation flags
///         ES_WINDOW_RGB     - specifies that the color buffer should have R,G,B channels
///         ES_WINDOW_ALPHA   - specifies that the color buffer should have alpha
///         ES_WINDOW_DEPTH   - specifies that a depth buffer should be created
///         ES_WINDOW_STENCIL - specifies that a stencil buffer should be created
///         ES_WINDOW_MULTISAMPLE - specifies that a multi-sample buffer should be created
/// \return GL_TRUE if window creation is succesful, GL_FALSE otherwise

//GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, const char *title, GLint width, GLint height, GLuint flags );
//GLboolean ESUTIL_API esCreateWindow ( ESContext *esContext, LPCTSTR title, GLint width, GLint height, GLuint flags )
//{
//   EGLint configAttribList[] =
//   {
//	   EGL_RED_SIZE,       5,
//	   EGL_GREEN_SIZE,     6,
//	   EGL_BLUE_SIZE,      5,
//	   EGL_ALPHA_SIZE,     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
//	   EGL_DEPTH_SIZE,     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
//	   EGL_STENCIL_SIZE,   (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
//	   EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
//	   EGL_NONE
//   };
//   EGLint surfaceAttribList[] =
//   {
//	   EGL_POST_SUB_BUFFER_SUPPORTED_NV, flags & (ES_WINDOW_POST_SUB_BUFFER_SUPPORTED) ? EGL_TRUE : EGL_FALSE,
//	   EGL_NONE, EGL_NONE
//   };
//
//   if ( esContext == NULL )
//   {
//	  return GL_FALSE;
//   }
//
//   esContext->width = width;
//   esContext->height = height;
//
//   if ( !WinCreate ( esContext, title) )
//   {
//	  return GL_FALSE;
//   }
//
//
//   if ( !CreateEGLContext ( esContext->hWnd,
//							&esContext->eglDisplay,
//							&esContext->eglContext,
//							&esContext->eglSurface,
//							configAttribList,
//							surfaceAttribList ) )
//   {
//	  return GL_FALSE;
//   }
//
//
//   return GL_TRUE;
//}

//
/// \brief Register a draw callback function to be used to render each frame
/// \param esContext Application context
/// \param drawFunc Draw callback function that will be used to render the scene
//

//void ESUTIL_API esRegisterDrawFunc ( ESContext *esContext, void ( ESCALLBACK *drawFunc ) ( ESContext * ) );
void ESUTIL_API esRegisterDrawFunc(ESContext* esContext, void (ESCALLBACK* drawFunc) (ESContext*))
{
	esContext->drawFunc = drawFunc;
}

//
/// \brief Register a callback function to be called on shutdown
/// \param esContext Application context
/// \param shutdownFunc Shutdown callback function
//

//void ESUTIL_API esRegisterShutdownFunc ( ESContext *esContext, void ( ESCALLBACK *shutdownFunc ) ( ESContext * ) );
void ESUTIL_API esRegisterShutdownFunc(ESContext* esContext, void (ESCALLBACK* shutdownFunc) (ESContext*))
{
	esContext->shutdownFunc = shutdownFunc;
}

//
/// \brief Register an update callback function to be used to update on each time step
/// \param esContext Application context
/// \param updateFunc Update callback function that will be used to render the scene
//

//void ESUTIL_API esRegisterUpdateFunc ( ESContext *esContext, void ( ESCALLBACK *updateFunc ) ( ESContext *, float ) );
void ESUTIL_API esRegisterUpdateFunc(ESContext* esContext, void (ESCALLBACK* updateFunc) (ESContext*, float))
{
	esContext->updateFunc = updateFunc;
}

//
/// \brief Register a keyboard input processing callback function
/// \param esContext Application context
/// \param keyFunc Key callback function for application processing of keyboard input
//

//void ESUTIL_API esRegisterKeyFunc ( ESContext *esContext, void ( ESCALLBACK *drawFunc ) ( ESContext *, unsigned char, int, int ) );
void ESUTIL_API esRegisterKeyFunc(ESContext* esContext, void (ESCALLBACK* keyFunc) (ESContext*, unsigned char, int, int))
{
	esContext->keyFunc = keyFunc;
}
//
/// \brief Log a message to the debug output for the platform
/// \param formatStr Format string for error log.
//

//void ESUTIL_API esLogMessage ( const char *formatStr, ... );
void ESUTIL_API esLogMessage(const char* formatStr, ...)
{
	va_list params;
	char buf[BUFSIZ];

	va_start(params, formatStr);
	//vsprintf_s ( buf, sizeof(buf),  formatStr, params );
	vsprintf(buf, formatStr, params);

	printf("%s", buf);
	
	std::string filepath = "/sdcard/Android/data/com.outputlog/files/log.txt";
	myLogger.outputLog(std::string(buf), filepath);

	va_end(params);
}

//
///
/// \brief Load a shader, check for compile errors, print error messages to output log
/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
/// \param shaderSrc Shader source string
/// \return A new shader object on success, 0 on failure
//

//GLuint ESUTIL_API esLoadShader ( GLenum type, const char *shaderSrc );
GLuint ESUTIL_API esLoadShader(GLenum type, const char* shaderSrc) {
	GLuint shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(type);
	if (shader == 0) return 0;
	// Load the shader sourcem 
	glShaderSource(shader, 1, &shaderSrc, NULL);
	// Compile the shader
	glCompileShader(shader);
	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			esLogMessage("Error compiling shader:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}


//
///
/// \brief Load a vertex and fragment shader, create a program object, link program.
///        Errors output to log.
/// \param vertShaderSrc Vertex shader source code
/// \param fragShaderSrc Fragment shader source code
/// \return A new program object linked with the vertex/fragment shader pair, 0 on failure
//

//GLuint ESUTIL_API esLoadProgram ( const char *vertShaderSrc, const char *fragShaderSrc );
GLuint ESUTIL_API esLoadProgram(const char* vertShaderSrc, const char* fragShaderSrc) {
	//https://git.okoyono.de/klaute/OPiGPGPUTest/commit/529a8700b73e5b60238d38f1a1f9f51684a11218
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint programObject;
	GLint linked;
	// Load the vertex/fragment shaders
	vertexShader = esLoadShader(GL_VERTEX_SHADER, vertShaderSrc);
	if (vertexShader == 0) return 0;
	fragmentShader = esLoadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader);
		return 0;
	}
	// Create the program object
	programObject = glCreateProgram();
	if (programObject == 0) return 0;
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	// Link the program
	glLinkProgram(programObject);
	// Check the link status
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
			esLogMessage("Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(programObject);
		return 0;
	}
	// Free up no longer needed shader resources
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return programObject;
}


//
/// \brief Generates geometry for a sphere.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list for a TRIANGLE_STRIP
/// \param numSlices The number of slices in the sphere
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLE_STRIP
//

//int ESUTIL_API esGenSphere ( int numSlices, float radius, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords, GLuint **indices );
int ESUTIL_API esGenSphere(int numSlices, float radius, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices)
{
	//int i;
	//int j;
	//int numParallels = numSlices / 2;
	//int numVertices = ( numParallels + 1 ) * ( numSlices + 1 );
	//int numIndices = numParallels * numSlices * 6;
	//float angleStep = (2.0f * ES_PI) / ((float) numSlices);

	//// Allocate memory for buffers
	//if ( vertices != NULL )
	//	*vertices = malloc ( sizeof(GLfloat) * 3 * numVertices );

	//if ( normals != NULL )
	//	*normals = malloc ( sizeof(GLfloat) * 3 * numVertices );

	//if ( texCoords != NULL )
	//	*texCoords = malloc ( sizeof(GLfloat) * 2 * numVertices );

	//if ( indices != NULL )
	//	*indices = malloc ( sizeof(GLuint) * numIndices );

	//for ( i = 0; i < numParallels + 1; i++ )
	//{
	//	for ( j = 0; j < numSlices + 1; j++ )
	//	{
	//		int vertex = ( i * (numSlices + 1) + j ) * 3;

	//		if ( vertices )
	//		{
	//			(*vertices)[vertex + 0] = radius * sinf ( angleStep * (float)i ) *
	//											   sinf ( angleStep * (float)j );
	//			(*vertices)[vertex + 1] = radius * cosf ( angleStep * (float)i );
	//			(*vertices)[vertex + 2] = radius * sinf ( angleStep * (float)i ) *
	//											   cosf ( angleStep * (float)j );
	//		}

	//		if ( normals )
	//		{
	//			(*normals)[vertex + 0] = (*vertices)[vertex + 0] / radius;
	//			(*normals)[vertex + 1] = (*vertices)[vertex + 1] / radius;
	//			(*normals)[vertex + 2] = (*vertices)[vertex + 2] / radius;
	//		}

	//		if ( texCoords )
	//		{
	//			int texIndex = ( i * (numSlices + 1) + j ) * 2;
	//			(*texCoords)[texIndex + 0] = (float) j / (float) numSlices;
	//			(*texCoords)[texIndex + 1] = ( 1.0f - (float) i ) / (float) (numParallels - 1 );
	//		}
	//	}
	//}

	//// Generate the indices
	//if ( indices != NULL )
	//{
	//	GLuint *indexBuf = (*indices);
	//	for ( i = 0; i < numParallels ; i++ )
	//	{
	//		for ( j = 0; j < numSlices; j++ )
	//		{
	//			*indexBuf++  = i * ( numSlices + 1 ) + j;
	//			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + j;
	//			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );

	//			*indexBuf++ = i * ( numSlices + 1 ) + j;
	//			*indexBuf++ = ( i + 1 ) * ( numSlices + 1 ) + ( j + 1 );
	//			*indexBuf++ = i * ( numSlices + 1 ) + ( j + 1 );
	//		}
	//	}
	//}

	//return numIndices;
	int slicesOver2 = numSlices / 2;
	int slicesOver2Minus1 = slicesOver2 - 1;
	int slicesOver2Minus2 = slicesOver2 - 2;
	free(*normals);
	GLfloat* n = *normals = (GLfloat*)malloc((numSlices * (slicesOver2Minus1)+2) * 3 * sizeof(GLfloat));
	n[0] = 0.0f;
	n[1] = 0.0f;
	n[2] = 1.0f;
	int countNormals = 1;
	for (int i = 1; i < slicesOver2; i++) {
		double latitude = M_PI * i / slicesOver2;
		double sineOfLatitude = sin(latitude);
		for (int j = 0; j < numSlices; j++) {
			double longitude = 2.0 * M_PI * j / numSlices;
			n[3 * countNormals] = cos(longitude) * sineOfLatitude;
			n[3 * countNormals + 1] = sin(longitude) * sineOfLatitude;
			n[3 * countNormals + 2] = cos(latitude);
			countNormals++;
		}
	}
	n[3 * countNormals] = 0.0f;
	n[3 * countNormals + 1] = 0.0f;
	n[3 * countNormals + 2] = -1.0f;
	free(*vertices);
	GLfloat* v = *vertices = (GLfloat*)malloc((numSlices * (slicesOver2Minus1)+2) * 3 * sizeof(GLfloat));
	for (int i = 0; i < (numSlices * (slicesOver2 - 1) + 2) * 3; i++) {
		v[i] = radius * n[i];
	}
	free(*indices);
	GLuint* index = *indices = (GLuint*)malloc(numSlices * (slicesOver2 - 1) * 6 * sizeof(GLuint));
	int countIndices = 0;
	for (int i = 0; i < numSlices; i++) {
		index[countIndices] = 0;					   countIndices++;
		index[countIndices] = i + 1;				   countIndices++;
		index[countIndices] = (i + 1) % numSlices + 1;	countIndices++;
	}
	for (int i = 0; i < slicesOver2Minus2; i++) {
		for (int j = 0; j < numSlices; j++) {
			index[countIndices] = i * numSlices + j + 1;							   countIndices++;
			index[countIndices] = i * numSlices + j + 1 + numSlices;		countIndices++;
			index[countIndices] = i * numSlices + (j + 1) % numSlices + numSlices + 1;	countIndices++;
			index[countIndices] = i * numSlices + j + 1;							   countIndices++;
			index[countIndices] = i * numSlices + (j + 1) % numSlices + numSlices + 1;	countIndices++;
			index[countIndices] = i * numSlices + (j + 1) % numSlices + 1;	countIndices++;
		}
	}
	for (int i = 0; i < numSlices; i++) {
		index[countIndices] = numSlices * slicesOver2Minus2 + 1 + i;				   countIndices++;
		index[countIndices] = numSlices * slicesOver2Minus1 + 1;					   countIndices++;
		index[countIndices] = numSlices * slicesOver2Minus2 + (1 + i) % numSlices + 1;	countIndices++;
	}
	return countIndices;
}

//
/// \brief Generates geometry for a cube.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list for a TRIANGLES
/// \param scale The size of the cube, use 1.0 for a unit cube.
/// \param vertices If not NULL, will contain array of float3 positions
/// \param normals If not NULL, will contain array of float3 normals
/// \param texCoords If not NULL, will contain array of float2 texCoords
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLES
//

//int ESUTIL_API esGenCube ( float scale, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords, GLuint **indices );
int ESUTIL_API esGenCube(float scale, GLfloat** vertices, GLfloat** normals, GLfloat** texCoords, GLuint** indices)
{
	int i;
	int numVertices = 24;
	int numIndices = 36;

	GLfloat cubeVerts[] =
	{
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			0.5f,  0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f,  0.5f, 0.5f,
			0.5f,  0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f,  0.5f,
			0.5f,  0.5f,  0.5f,
			0.5f,  0.5f, -0.5f,
	};

	GLfloat cubeNormals[] =
	{
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
	};

	GLfloat cubeTex[] =
	{
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
	};

	// Allocate memory for buffers
	if (vertices != NULL)
	{
		*vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);
		memcpy(*vertices, cubeVerts, sizeof(cubeVerts));
		for (i = 0; i < numVertices * 3; i++)
		{
			(*vertices)[i] *= scale;
		}
	}

	if (normals != NULL)
	{
		*normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);
		memcpy(*normals, cubeNormals, sizeof(cubeNormals));
	}

	if (texCoords != NULL)
	{
		*texCoords = (GLfloat*)malloc(sizeof(GLfloat) * 2 * numVertices);
		memcpy(*texCoords, cubeTex, sizeof(cubeTex));
	}


	// Generate the indices
	if (indices != NULL)
	{
		GLuint cubeIndices[] =
		{
				0, 2, 1,
				0, 3, 2,
				4, 5, 6,
				4, 6, 7,
				8, 9, 10,
				8, 10, 11,
				12, 15, 14,
				12, 14, 13,
				16, 17, 18,
				16, 18, 19,
				20, 23, 22,
				20, 22, 21
		};

		*indices = (GLuint*)malloc(sizeof(GLuint) * numIndices);
		memcpy(*indices, cubeIndices, sizeof(cubeIndices));
	}

	return numIndices;
}

//
/// \brief Generates a square grid consisting of triangles.  Allocates memory for the vertex data and stores
///        the results in the arrays.  Generate index list as TRIANGLES.
/// \param size create a grid of size by size (number of triangles = (size-1)*(size-1)*2)
/// \param vertices If not NULL, will contain array of float3 positions
/// \param indices If not NULL, will contain the array of indices for the triangle strip
/// \return The number of indices required for rendering the buffers (the number of indices stored in the indices array
///         if it is not NULL ) as a GL_TRIANGLES
//

//int ESUTIL_API esGenSquareGrid ( int size, GLfloat **vertices, GLuint **indices );
int ESUTIL_API esGenSquareGrid(int size, GLfloat** vertices, GLuint** indices)
{
	int i, j;
	int numIndices = (size - 1) * (size - 1) * 2 * 3;

	// Allocate memory for buffers
	if (vertices != NULL)
	{
		int numVertices = size * size;
		float stepSize = (float)size - 1;
		*vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * numVertices);

		for (i = 0; i < size; ++i) // row
		{
			for (j = 0; j < size; ++j) // column
			{
				(*vertices)[3 * (j + i * size)] = i / stepSize;
				(*vertices)[3 * (j + i * size) + 1] = j / stepSize;
				(*vertices)[3 * (j + i * size) + 2] = 0.0f;
			}
		}
	}

	// Generate the indices
	if (indices != NULL)
	{
		*indices = (GLuint*)malloc(sizeof(GLuint) * numIndices);

		for (i = 0; i < size - 1; ++i)
		{
			for (j = 0; j < size - 1; ++j)
			{
				// two triangles per quad
				(*indices)[6 * (j + i * (size - 1))] = j + (i) * (size);
				(*indices)[6 * (j + i * (size - 1)) + 1] = j + (i) * (size)+1;
				(*indices)[6 * (j + i * (size - 1)) + 2] = j + (i + 1) * (size)+1;

				(*indices)[6 * (j + i * (size - 1)) + 3] = j + (i) * (size);
				(*indices)[6 * (j + i * (size - 1)) + 4] = j + (i + 1) * (size)+1;
				(*indices)[6 * (j + i * (size - 1)) + 5] = j + (i + 1) * (size);
			}
		}
	}

	return numIndices;
}

//
/// \brief Loads a 8-bit, 24-bit or 32-bit TGA image from a file
/// \param ioContext Context related to IO facility on the platform
/// \param fileName Name of the file on disk
/// \param width Width of loaded image in pixels
/// \param height Height of loaded image in pixels
///  \return Pointer to loaded image.  NULL on failure.
//

//char *ESUTIL_API esLoadTGA ( void *ioContext, const char *fileName, int *width, int *height );
//char* ESUTIL_API esLoadTGA ( char *fileName, int *width, int *height )
//{
//	char *buffer;
//
//	if ( WinTGALoad ( fileName, &buffer, width, height ) )
//	{
//		return buffer;
//	}
//
//	return NULL;
//}


//
/// \brief Multiply matrix specified by result with a scaling matrix and return new matrix in result
/// \param result Specifies the input matrix.  Scaled matrix is returned in result.
/// \param sx, sy, sz Scale factors along the x, y and z axes respectively
//

//void ESUTIL_API esScale ( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz );
void ESUTIL_API esScale(ESMatrix* result, GLfloat sx, GLfloat sy, GLfloat sz)
{
	result->m[0][0] *= sx;
	result->m[0][1] *= sx;
	result->m[0][2] *= sx;
	result->m[0][3] *= sx;

	result->m[1][0] *= sy;
	result->m[1][1] *= sy;
	result->m[1][2] *= sy;
	result->m[1][3] *= sy;

	result->m[2][0] *= sz;
	result->m[2][1] *= sz;
	result->m[2][2] *= sz;
	result->m[2][3] *= sz;
}

//
/// \brief Multiply matrix specified by result with a translation matrix and return new matrix in result
/// \param result Specifies the input matrix.  Translated matrix is returned in result.
/// \param tx, ty, tz Scale factors along the x, y and z axes respectively
//

//void ESUTIL_API esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );
void ESUTIL_API esTranslate(ESMatrix* result, GLfloat tx, GLfloat ty, GLfloat tz)
{
	result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
	result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
	result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
	result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

//
/// \brief Multiply matrix specified by result with a rotation matrix and return new matrix in result
/// \param result Specifies the input matrix.  Rotated matrix is returned in result.
/// \param angle Specifies the angle of rotation, in degrees.
/// \param x, y, z Specify the x, y and z coordinates of a vector, respectively
//

//void ESUTIL_API esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
void ESUTIL_API esRotate(ESMatrix* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	GLfloat sinAngle, cosAngle;
	GLfloat mag = sqrtf(x * x + y * y + z * z);

	sinAngle = sinf(angle * M_PI / 180.0f);
	cosAngle = cosf(angle * M_PI / 180.0f);
	if (mag > 0.0f)
	{
		GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
		GLfloat oneMinusCos;
		ESMatrix rotMat;

		x /= mag;
		y /= mag;
		z /= mag;

		xx = x * x;
		yy = y * y;
		zz = z * z;
		xy = x * y;
		yz = y * z;
		zx = z * x;
		xs = x * sinAngle;
		ys = y * sinAngle;
		zs = z * sinAngle;
		oneMinusCos = 1.0f - cosAngle;

		rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
		rotMat.m[0][1] = (oneMinusCos * xy) - zs;
		rotMat.m[0][2] = (oneMinusCos * zx) + ys;
		rotMat.m[0][3] = 0.0F;

		rotMat.m[1][0] = (oneMinusCos * xy) + zs;
		rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
		rotMat.m[1][2] = (oneMinusCos * yz) - xs;
		rotMat.m[1][3] = 0.0F;

		rotMat.m[2][0] = (oneMinusCos * zx) - ys;
		rotMat.m[2][1] = (oneMinusCos * yz) + xs;
		rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
		rotMat.m[2][3] = 0.0F;

		rotMat.m[3][0] = 0.0F;
		rotMat.m[3][1] = 0.0F;
		rotMat.m[3][2] = 0.0F;
		rotMat.m[3][3] = 1.0F;

		esMatrixMultiply(result, &rotMat, result);
	}
}

//
/// \brief Multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  New matrix is returned in result.
/// \param left, right Coordinates for the left and right vertical clipping planes
/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
/// \param nearZ, farZ Distances to the near and far depth clipping planes.  Both distances must be positive.
//

//void ESUTIL_API esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
void ESUTIL_API esFrustum(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
	float       deltaX = right - left;
	float       deltaY = top - bottom;
	float       deltaZ = farZ - nearZ;
	ESMatrix    frust;

	if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
		(deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
		return;

	frust.m[0][0] = 2.0f * nearZ / deltaX;
	frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

	frust.m[1][1] = 2.0f * nearZ / deltaY;
	frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

	frust.m[2][0] = (right + left) / deltaX;
	frust.m[2][1] = (top + bottom) / deltaY;
	frust.m[2][2] = -(nearZ + farZ) / deltaZ;
	frust.m[2][3] = -1.0f;

	frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
	frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

	esMatrixMultiply(result, &frust, result);
}

//
/// \brief Multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  New matrix is returned in result.
/// \param fovy Field of view y angle in degrees
/// \param aspect Aspect ratio of screen
/// \param nearZ Near plane distance
/// \param farZ Far plane distance
//

//void ESUTIL_API esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ );
void ESUTIL_API esPerspective(ESMatrix* result, float fovy, float aspect, float nearZ, float farZ)
{
	GLfloat frustumW, frustumH;

	frustumH = tanf(fovy / 360.0f * M_PI) * nearZ;
	frustumW = frustumH * aspect;

	esFrustum(result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
}

//
/// \brief Multiply matrix specified by result with a perspective matrix and return new matrix in result
/// \param result Specifies the input matrix.  New matrix is returned in result.
/// \param left, right Coordinates for the left and right vertical clipping planes
/// \param bottom, top Coordinates for the bottom and top horizontal clipping planes
/// \param nearZ, farZ Distances to the near and far depth clipping planes.  These values are negative if plane is behind the viewer
//

//void ESUTIL_API esOrtho ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );
void ESUTIL_API esOrtho(ESMatrix* result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
	float       deltaX = right - left;
	float       deltaY = top - bottom;
	float       deltaZ = farZ - nearZ;
	ESMatrix    ortho;

	if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f))
		return;

	esMatrixLoadIdentity(&ortho);
	ortho.m[0][0] = 2.0f / deltaX;
	ortho.m[3][0] = -(right + left) / deltaX;
	ortho.m[1][1] = 2.0f / deltaY;
	ortho.m[3][1] = -(top + bottom) / deltaY;
	ortho.m[2][2] = -2.0f / deltaZ;
	ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

	esMatrixMultiply(result, &ortho, result);
}

//
/// \brief Perform the following operation - result matrix = srcA matrix * srcB matrix
/// \param result Returns multiplied matrix
/// \param srcA, srcB Input matrices to be multiplied
//

//void ESUTIL_API esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );
void ESUTIL_API esMatrixMultiply(ESMatrix* result, ESMatrix* srcA, ESMatrix* srcB)
{
	ESMatrix    tmp;
	int         i;

	for (i = 0; i < 4; i++)
	{
		tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) +
			(srcA->m[i][1] * srcB->m[1][0]) +
			(srcA->m[i][2] * srcB->m[2][0]) +
			(srcA->m[i][3] * srcB->m[3][0]);

		tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) +
			(srcA->m[i][1] * srcB->m[1][1]) +
			(srcA->m[i][2] * srcB->m[2][1]) +
			(srcA->m[i][3] * srcB->m[3][1]);

		tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) +
			(srcA->m[i][1] * srcB->m[1][2]) +
			(srcA->m[i][2] * srcB->m[2][2]) +
			(srcA->m[i][3] * srcB->m[3][2]);

		tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) +
			(srcA->m[i][1] * srcB->m[1][3]) +
			(srcA->m[i][2] * srcB->m[2][3]) +
			(srcA->m[i][3] * srcB->m[3][3]);
	}
	memcpy(result, &tmp, sizeof(ESMatrix));
}

//
//// \brief Return an identity matrix
//// \param result Returns identity matrix
//

//void ESUTIL_API esMatrixLoadIdentity ( ESMatrix *result );
void ESUTIL_API esMatrixLoadIdentity(ESMatrix* result)
{
	memset(result, 0x0, sizeof(ESMatrix));
	result->m[0][0] = 1.0f;
	result->m[1][1] = 1.0f;
	result->m[2][2] = 1.0f;
	result->m[3][3] = 1.0f;
}

//
/// \brief Generate a transformation matrix from eye position, look at and up vectors
/// \param result Returns transformation matrix
/// \param posX, posY, posZ           eye position
/// \param lookAtX, lookAtY, lookAtZ  look at vector
/// \param upX, upY, upZ              up vector
//
void ESUTIL_API esMatrixLookAt(ESMatrix* result, float posX, float posY, float posZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ) {

}

#endif