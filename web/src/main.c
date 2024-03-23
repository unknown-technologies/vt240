#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include <math.h>

#include <emscripten/html5.h>

#include "types.h"
#include "vt.h"
#include "renderer.h"
#include "keyboard.h"

#define	SCREEN_WIDTH		800
#define	SCREEN_HEIGHT		480

static bool enable_glow = true;

static int screen_width;
static int screen_height;

static VT240 vt;
static VTRenderer renderer;

static unsigned long current_time = 0;

#ifdef NDEBUG
#define GL_ERROR()
#else
#define	GL_ERROR()	check_error(__FILE__, __LINE__)

void check_error(const char* filename, unsigned int line)
{
	GLenum error = glGetError();
	switch(error) {
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
			printf("%s:%u: Error: GL_INVALID_ENUM\n", filename, line);
			break;
		case GL_INVALID_VALUE:
			printf("%s:%u: Error: GL_INVALID_VALUE\n", filename, line);
			break;
		case GL_INVALID_OPERATION:
			printf("%s:%u: Error: GL_INVALID_OPERATION\n", filename, line);
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			printf("%s:%u: Error: GL_INVALID_FRAMEBUFFER_OPERATION\n", filename, line);
			break;
		case GL_OUT_OF_MEMORY:
			printf("%s:%u: Error: GL_OUT_OF_MEMORY\n", filename, line);
			exit(1);
			break;
		case GL_STACK_UNDERFLOW:
			printf("%s:%u: Error: GL_STACK_UNDERFLOW\n", filename, line);
			break;
		case GL_STACK_OVERFLOW:
			printf("%s:%u: Error: GL_STACK_OVERFLOW\n", filename, line);
			break;
		default:
			printf("%s:%u: Unknown error 0x%X\n", filename, line, error);
	}
}
#endif

static void check_for_reshape()
{
	double canvas_width;
	double canvas_height;

	emscripten_get_element_css_size("#canvas", &canvas_width, &canvas_height);

	if((int) canvas_width != screen_width || (int) canvas_height != screen_height) {
		glutReshapeWindow((int) canvas_width, (int) canvas_height);
	}
}

__attribute__((weak))
void SYSReceive(unsigned char c)
{
	// ignore XON/XOFF
	if(c == DC1 || c == DC3) {
		return;
	}

	VT240Receive(&vt, c);
}

__attribute__((weak))
void SYSInit(void)
{
	// empty
}

__attribute__((weak))
void SYSProcess(unsigned long dt)
{
	// empty
}

void SYSSend(unsigned char c)
{
	VT240Receive(&vt, c);
}

void process(void)
{
	unsigned long now = glutGet(GLUT_ELAPSED_TIME);

	unsigned long dt = now - current_time;

	SYSProcess(dt);
	VT240Process(&vt, dt);
	VTProcess(&renderer, dt);

	current_time = now;
}

void reshape_func(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

void display_func(void)
{
	GL_ERROR();
	process();

	check_for_reshape();

	VTRender(&renderer, screen_width, screen_height);
	GL_ERROR();

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	// glutInitContextVersion(4, 6);
	// glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	// glutInitWindowPosition(100, 100);
	glutCreateWindow("VT240");

	const unsigned char* gl_vendor = glGetString(GL_VENDOR);
	const unsigned char* gl_renderer = glGetString(GL_RENDERER);
	const unsigned char* gl_version = glGetString(GL_VERSION);
	const unsigned char* gl_glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("GL Vendor:    %s\n", gl_vendor);
	printf("GL Renderer:  %s\n", gl_renderer);
	printf("GL Version:   %s\n", gl_version);
	printf("GLSL Version: %s\n", gl_glsl_version);

	printf("using depth buffer with %d bit\n", glutGet(GLUT_WINDOW_DEPTH_SIZE));

	int num_ext = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
	for(int i = 0; i < num_ext; i++) {
		if(!strcmp((const char*) glGetStringi(GL_EXTENSIONS, i), "GL_ARB_compatibility")) {
			printf("GL Compatiblity Profile\n");
			break;
		}
	}

	GL_ERROR();

	glutReshapeFunc(reshape_func);
	glutDisplayFunc(display_func);
	glutIdleFunc(display_func);

	VT240Init(&vt);
	vt.rx = SYSReceive;

	VTInitRenderer(&renderer, &vt);
	VTEnableGlow(&renderer, enable_glow);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	GL_ERROR();

	SYSInit();

	// glutSetCursor(GLUT_CURSOR_NONE);

	current_time = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

	return 0;
}
