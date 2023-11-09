/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <EGL/egl.h>

#include "OpenGLES10Helpers.h"

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))

const int PORTRAIT = 0;
const int LANDSCAPE = 1;
int orientation = PORTRAIT;

///
// Cleanup
//
void ShutDown ( ESContext *esContext )
{
	
}

// Our saved state data.
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
 * Shared state for our app.
 */
struct engine
{
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	const ASensor* rotationVectorSensor;
	ASensorEventQueue* accelerometerSensorEventQueue;
	ASensorEventQueue* rotationVectorSensorEventQueue;

    int animating = 1;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	EGLint width;
	EGLint height;
	vec3 accelerometerVectorInput;
	vec4 rotationVectorInput;
	int framesDrawn = 0;
	struct saved_state state;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_NONE
	};

	EGLint format, numConfigs;
	EGLConfig config;

	engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(engine->display, NULL, NULL);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	eglChooseConfig(engine->display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(engine->display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	engine->surface = eglCreateWindowSurface(engine->display, config, engine->app->window, NULL);

	EGLint contextAttribs[] =
	{
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
	};
	engine->context = eglCreateContext(engine->display, config, NULL, contextAttribs);

	if (eglMakeCurrent(engine->display, engine->surface, engine->surface, engine->context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}
	
	gl.initializeShaders();

	int major = 0;
	int minor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	std::string filepath = "/sdcard/Android/data/com.outputlog/files/log.txt";
	myLogger.outputLog(std::to_string(major) + "." + std::to_string(minor), filepath);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnableVertexAttribArray ( POSITION_ATTRIBUTE_LOCATION );

	return 0;
}

static void engine_update_frame(struct engine* engine)
{
	if (engine->framesDrawn == 0)
	{
		
	}
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine)
{
	if (engine->display == NULL)
	{
		// No display.
		return;
	}
	
	eglQuerySurface(engine->display, engine->surface, EGL_WIDTH, &engine->width);
	eglQuerySurface(engine->display, engine->surface, EGL_HEIGHT, &engine->height);
	// Set the viewport
	glViewport(0, 0, engine->width, engine->height);
	// Compute the window aspect ratio
	GLfloat aspect = (GLfloat)engine->width / (GLfloat)engine->height;
	// Generate a perspective matrix with a 90-degree FOV and near and far clip planes at 0.1 and 1000.0
	float maxViewAngle = 45.0f;
	float zNear = 0.1f;
	float zFar = 1000.0f;
	float verticalScreenAngle = (aspect < 1.0f) ? maxViewAngle : 2.0 * atan(tan(maxViewAngle / 2.0 * M_PI / 180.0) / aspect) / M_PI * 180.0;
	gl.gluPerspectivef(verticalScreenAngle, aspect, zNear, zFar);

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl.glLoadIdentity();
    
	vec3 cameraPosition = quaternionTo3x3(engine->rotationVectorInput) * vec3(0.0f, 0.0f, 10.0f);
	gl.rotateTransformationMatrix(engine->rotationVectorInput);
	gl.glTranslatef(-cameraPosition.x, -cameraPosition.y, -cameraPosition.z);
	
	gl.drawCube(vec3(0.0f, 0.0f, 0.0f));
	gl.drawCube(vec3(0.0f, 0.0f, 2.0f));
	
	engine->framesDrawn++;
	
	eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine)
{
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
        engine->animating = 1;
        engine->state.x = AMotionEvent_getX(event, 0);
        engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* engine = static_cast<struct engine*>(app->userData);
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		/////////// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		engine->animating = 1;
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->accelerometerSensorEventQueue, engine->accelerometerSensor);
			// We'd like to get 60 events per second (in us).
			ASensorEventQueue_setEventRate(engine->accelerometerSensorEventQueue, engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// When our app gains focus, we start monitoring the rotation vector.
		if (engine->rotationVectorSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->rotationVectorSensorEventQueue, engine->rotationVectorSensor);
			// We'd like to get 60 events per second (in us).
			ASensorEventQueue_setEventRate(engine->rotationVectorSensorEventQueue, engine->rotationVectorSensor, (1000L / 60) * 1000);
		}
		engine->animating = 1;
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->accelerometerSensorEventQueue, engine->accelerometerSensor);
		}
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// When our app loses focus, we stop monitoring the rotation vector.
		// This is to avoid consuming battery while not being used.
		if (engine->rotationVectorSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->rotationVectorSensorEventQueue, engine->rotationVectorSensor);
		}
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// Also stop animating.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	case APP_CMD_INPUT_CHANGED:
		break;
	case APP_CMD_CONFIG_CHANGED://screen rotated
		break;
	case APP_CMD_START:
		break;
	case APP_CMD_RESUME:
		break;
	case APP_CMD_PAUSE://back button pressed
		break;
	default:
		break;
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	}
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state)
{
	struct engine engine;

	// Make sure glue isn't stripped.
	app_dummy();

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// Prepare to monitor sensors.
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	engine.rotationVectorSensor = ASensorManager_getDefaultSensor(engine.sensorManager, ASENSOR_TYPE_ROTATION_VECTOR);
	engine.accelerometerSensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);
	engine.rotationVectorSensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

	// loop waiting for stuff to do.
	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident=ALooper_pollAll(0, NULL, &events, reinterpret_cast<void**>(&source))) >= 0)
		{
			// Process this event.
			if (source != NULL)
			{
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (engine.accelerometerSensor != NULL) {
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(engine.accelerometerSensorEventQueue, &event, 1) > 0) {
					//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
					//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
					switch (orientation) {
					case PORTRAIT:
						engine.accelerometerVectorInput = vec3(event.acceleration.x, event.acceleration.y, event.acceleration.z);
						break;
					case LANDSCAPE:
						engine.accelerometerVectorInput = vec3(-event.acceleration.y, event.acceleration.x, event.acceleration.z);
						break;
					}
					//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
				}
			}

			// If a sensor has data, process it now.
			if (engine.rotationVectorSensor != NULL) {
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(engine.rotationVectorSensorEventQueue, &event, 1) > 0) {
					//LOGI("rotationVector: azimuth=%f pitch=%f roll=%f" axis=%f, event.vector.azimuth, event.vector.pitch, event.vector.roll, event.data[3]);
					//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
					switch (orientation) {
					case PORTRAIT:
						engine.rotationVectorInput = vec4(event.vector.azimuth, event.vector.pitch, event.vector.roll, event.data[3]);
						break;
					case LANDSCAPE:
						engine.rotationVectorInput = vec4(-event.vector.pitch, event.vector.azimuth, event.vector.roll, event.data[3]);
						break;
					}
					//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0)
			{
				engine_term_display(&engine);
				return;
			}
		}

		// Done with events; draw next animation frame.
		// Drawing is throttled to the screen update rate, so there
		// is no need to do timing here.
		engine_update_frame(&engine);
		engine_draw_frame(&engine);
	}
}
