#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include <math.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

#include "types.h"
#include "vt.h"
#include "renderer.h"
#include "keyboard.h"

#define	SCREEN_WIDTH		800
#define	SCREEN_HEIGHT		480

static bool enable_glow = true;
static bool is_fullscreen = false;

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

#ifdef __EMSCRIPTEN__
static void check_for_reshape()
{
	double canvas_width;
	double canvas_height;

	emscripten_get_element_css_size("#canvas", &canvas_width, &canvas_height);

	if((int) canvas_width != screen_width || (int) canvas_height != screen_height) {
		glutReshapeWindow((int) canvas_width, (int) canvas_height);
	}
}
#endif

void process(void)
{
	unsigned long now = glutGet(GLUT_ELAPSED_TIME);

	unsigned long dt = now - current_time;

	VT240Process(&vt, dt);
	VTProcess(&renderer, dt);

	current_time = now;
}

void reshape_func(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

void kb_func(unsigned char key, int x, int y)
{
	if(key == BS) {
		VT240KeyDown(&vt, DEL);
	} else {
		VT240KeyDown(&vt, key);
	}
}

void kb_up_func(unsigned char key, int x, int y)
{
	if(key == BS) {
		VT240KeyUp(&vt, DEL);
	} else {
		VT240KeyUp(&vt, key);
	}
}

void special_func(int key, int x, int y)
{
	switch(key) {
#ifdef __EMSCRIPTEN__
		case 111:
			VT240KeyDown(&vt, VT240_KEY_REMOVE);
			break;
		case 120:
			VT240KeyDown(&vt, DEL);
			break;
#endif
		case GLUT_KEY_F1:
			VT240KeyDown(&vt, VT240_KEY_HOLD_SCREEN);
			break;
		case GLUT_KEY_F2:
			// VT240KeyDown(&vt, VT240_KEY_PRINT_SCREEN);
			if(is_fullscreen) {
				glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
			} else {
				glutFullScreen();
			}
			is_fullscreen = !is_fullscreen;
			break;
		case GLUT_KEY_F3:
			VT240KeyDown(&vt, VT240_KEY_SET_UP);
			break;
		case GLUT_KEY_F4:
			VT240KeyDown(&vt, VT240_KEY_DATA_TALK);
			break;
		case GLUT_KEY_F5:
			// VT240KeyDown(&vt, VT240_KEY_BREAK);
			// exit(0);
			break;
		case GLUT_KEY_F6:
			VT240KeyDown(&vt, VT240_KEY_F6);
			break;
		case GLUT_KEY_F7:
			VT240KeyDown(&vt, VT240_KEY_F7);
			break;
		case GLUT_KEY_F8:
			VT240KeyDown(&vt, VT240_KEY_F8);
			break;
		case GLUT_KEY_F9:
			VT240KeyDown(&vt, VT240_KEY_F9);
			break;
		case GLUT_KEY_F10:
			VT240KeyDown(&vt, VT240_KEY_F10);
			break;
		case GLUT_KEY_F11:
			VT240KeyDown(&vt, VT240_KEY_F11);
			break;
		case GLUT_KEY_F12:
			VT240KeyDown(&vt, VT240_KEY_F12);
			break;
		case GLUT_KEY_LEFT:
			VT240KeyDown(&vt, VT240_KEY_LEFT);
			break;
		case GLUT_KEY_UP:
			VT240KeyDown(&vt, VT240_KEY_UP);
			break;
		case GLUT_KEY_RIGHT:
			VT240KeyDown(&vt, VT240_KEY_RIGHT);
			break;
		case GLUT_KEY_DOWN:
			VT240KeyDown(&vt, VT240_KEY_DOWN);
			break;
		case GLUT_KEY_PAGE_UP:
			VT240KeyDown(&vt, VT240_KEY_PREV_SCREEN);
			break;
		case GLUT_KEY_PAGE_DOWN:
			VT240KeyDown(&vt, VT240_KEY_NEXT_SCREEN);
			break;
		case GLUT_KEY_HOME:
			VT240KeyDown(&vt, VT240_KEY_FIND);
			break;
		case GLUT_KEY_END:
			VT240KeyDown(&vt, VT240_KEY_SELECT);
			break;
		case GLUT_KEY_INSERT:
			VT240KeyDown(&vt, VT240_KEY_INSERT);
			break;
	}
}

void special_up_func(int key, int x, int y)
{
	switch(key) {
#ifdef __EMSCRIPTEN__
		case 111:
			VT240KeyUp(&vt, VT240_KEY_REMOVE);
			break;
		case 120:
			VT240KeyUp(&vt, DEL);
			break;
#endif
		case GLUT_KEY_F1:
			VT240KeyUp(&vt, VT240_KEY_HOLD_SCREEN);
			break;
		case GLUT_KEY_F2:
			// VT240KeyUp(&vt, VT240_KEY_PRINT_SCREEN);
			break;
		case GLUT_KEY_F3:
			VT240KeyUp(&vt, VT240_KEY_SET_UP);
			break;
		case GLUT_KEY_F4:
			VT240KeyUp(&vt, VT240_KEY_SET_UP);
			break;
		case GLUT_KEY_F5:
			// VT240KeyUp(&vt, VT240_KEY_BREAK);
			break;
		case GLUT_KEY_F6:
			VT240KeyUp(&vt, VT240_KEY_F6);
			break;
		case GLUT_KEY_F7:
			VT240KeyUp(&vt, VT240_KEY_F7);
			break;
		case GLUT_KEY_F8:
			VT240KeyUp(&vt, VT240_KEY_F8);
			break;
		case GLUT_KEY_F9:
			VT240KeyUp(&vt, VT240_KEY_F9);
			break;
		case GLUT_KEY_F10:
			VT240KeyUp(&vt, VT240_KEY_F10);
			break;
		case GLUT_KEY_F11:
			VT240KeyUp(&vt, VT240_KEY_F11);
			break;
		case GLUT_KEY_F12:
			VT240KeyUp(&vt, VT240_KEY_F12);
			break;
		case GLUT_KEY_LEFT:
			VT240KeyUp(&vt, VT240_KEY_LEFT);
			break;
		case GLUT_KEY_UP:
			VT240KeyUp(&vt, VT240_KEY_UP);
			break;
		case GLUT_KEY_RIGHT:
			VT240KeyUp(&vt, VT240_KEY_RIGHT);
			break;
		case GLUT_KEY_DOWN:
			VT240KeyUp(&vt, VT240_KEY_DOWN);
			break;
		case GLUT_KEY_PAGE_UP:
			VT240KeyUp(&vt, VT240_KEY_PREV_SCREEN);
			break;
		case GLUT_KEY_PAGE_DOWN:
			VT240KeyUp(&vt, VT240_KEY_NEXT_SCREEN);
			break;
		case GLUT_KEY_HOME:
			VT240KeyUp(&vt, VT240_KEY_FIND);
			break;
		case GLUT_KEY_END:
			VT240KeyUp(&vt, VT240_KEY_SELECT);
			break;
		case GLUT_KEY_INSERT:
			VT240KeyUp(&vt, VT240_KEY_INSERT);
			break;
	}
}

void print_ch(unsigned char c)
{
	// ignore XON/XOFF
	if(c == DC1 || c == DC3) {
		return;
	}

	VT240Receive(&vt, c);
}

void display_func(void)
{
	GL_ERROR();
	process();

#ifdef __EMSCRIPTEN__
	check_for_reshape();
#endif

	VTRender(&renderer, screen_width, screen_height);
	GL_ERROR();

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
#ifdef __EMSCRIPTEN__
	// hack to get ctrl-key work in a somewhat useful way
	EM_ASM(
		let __old_getASCIIKey = GLUT.getASCIIKey;
		GLUT.getASCIIKey = function(e) {
			if(e.ctrlKey) {
				let evt = {};
				evt.ctrlKey = false;
				evt.altKey = false;
				evt.metaKey = false;
				evt.shiftKey = e.shiftKey;
				evt.keyCode = e.keyCode;
				let result = __old_getASCIIKey(evt);
				if(result != null && result >= 97 && result <= 122) {
					return result - 97;
				} else {
					return null;
				}
			} else {
				return __old_getASCIIKey(e);
			}
		};
	);
#endif
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

	glutKeyboardFunc(kb_func);
	glutKeyboardUpFunc(kb_up_func);
	glutSpecialFunc(special_func);
	glutSpecialUpFunc(special_up_func);

	//glutIgnoreKeyRepeat(1);

	VT240Init(&vt);
	vt.rx = print_ch;

	VTInitRenderer(&renderer, &vt);
	VTEnableGlow(&renderer, enable_glow);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	GL_ERROR();

	// glutSetCursor(GLUT_CURSOR_NONE);

	current_time = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

	return 0;
}
