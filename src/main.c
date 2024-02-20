#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <math.h>

#include "types.h"
#include "vt.h"
#include "renderer.h"
#include "telnet.h"

#define	SCREEN_WIDTH		800
#define	SCREEN_HEIGHT		480

static bool enable_glow = true;
static bool use_telnet = false;
static bool is_fullscreen = false;

static int screen_width;
static int screen_height;

static VT240 vt;
static VTRenderer renderer;
static TELNET telnet;

static unsigned long time = 0;

#ifdef _WIN32
PFNGLGENFRAMEBUFFERSPROC	glGenFramebuffers;
PFNGLBINDFRAMEBUFFERPROC	glBindFramebuffer;
PFNGLFRAMEBUFFERTEXTURE2DPROC	glFramebufferTexture2D;
PFNGLCHECKFRAMEBUFFERSTATUSPROC	glCheckFramebufferStatus;

static void load_gl_extensions(void)
{
	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
}
#endif

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

void process(void)
{
	unsigned long now = glutGet(GLUT_ELAPSED_TIME);

	unsigned long dt = now - time;

	if(use_telnet) {
		TELNETPoll(&telnet);
	}

	VT240Process(&vt, dt);
	VTProcess(&renderer, dt);

	time = now;
}

void reshape_func(int width, int height)
{
	screen_width = width;
	screen_height = height;
}

void kb_func(unsigned char key, int x, int y)
{
	if(key == 0x7F) {
		VT240KeyDown(&vt, VT240_KEY_REMOVE);
	} else {
		VT240KeyDown(&vt, key);
	}
}

void kb_up_func(unsigned char key, int x, int y)
{
	if(key == 0x7F) {
		VT240KeyUp(&vt, VT240_KEY_REMOVE);
	} else {
		VT240KeyUp(&vt, key);
	}
}

void special_func(int key, int x, int y)
{
	switch(key) {
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
			if(use_telnet) {
				TELNETDisconnect(&telnet);
			}
			exit(0);
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

	putc(c, stdout);
	fflush(stdout);
}

void display_func(void)
{
	GL_ERROR();
	process();

	VTRender(&renderer, screen_width, screen_height);
	GL_ERROR();

	glutSwapBuffers();
}

static void telnet_rx(unsigned char c)
{
	VT240Receive(&vt, c);
}

static void telnet_tx(unsigned char c)
{
	TELNETSend(&telnet, c);
}

int main(int argc, char** argv)
{
	const char* self = *argv;
	char* modestring = NULL;
	int use_game_mode = 0;
	argc--;
	argv++;
	if(argc > 1 && !strcmp(argv[0], "-f")) {
		modestring = argv[1];
		argc -= 2;
		argv += 2;
	}
	if(argc > 0 && !strcmp(argv[0], "-g")) {
		enable_glow = false;
		argc--;
		argv++;
	}
	if(argc != 0 && argc != 2) {
		printf("Usage: %s [-f modestring] [-g] [hostname port]\n", self);
		exit(0);
	}

	glutInit(&argc, argv);
	// glutInitContextVersion(4, 6);
	// glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	if(modestring) {
		glutGameModeString(modestring);
		int possible = glutGameModeGet(GLUT_GAME_MODE_POSSIBLE);
		if(possible) {
			glutEnterGameMode();
			use_game_mode = 1;
		} else {
			printf("Cannot use mode string \"%s\"\n", modestring);
		}
	}

	if(!use_game_mode) {
		glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		// glutInitWindowPosition(100, 100);
		glutCreateWindow("VT240");
	}

	const unsigned char* gl_vendor = glGetString(GL_VENDOR);
	const unsigned char* gl_renderer = glGetString(GL_RENDERER);
	const unsigned char* gl_version = glGetString(GL_VERSION);
	const unsigned char* gl_glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("GL Vendor:    %s\n", gl_vendor);
	printf("GL Renderer:  %s\n", gl_renderer);
	printf("GL Version:   %s\n", gl_version);
	printf("GLSL Version: %s\n", gl_glsl_version);

	printf("using depth buffer with %d bit\n", glutGet(GLUT_WINDOW_DEPTH_SIZE));

#ifdef _WIN32
	load_gl_extensions();
#endif

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

	glutIgnoreKeyRepeat(1);

	VT240Init(&vt, 80, 24);
	vt.rx = print_ch;
	vt.mode &= ~SRM;

	VTInitRenderer(&renderer, &vt);
	VTEnableGlow(&renderer, enable_glow);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	GL_ERROR();

	if(argc == 2) {
		const char* hostname = argv[0];
		const int port = atoi(argv[1]);
		char buf[256];
		use_telnet = true;

		VT240ReceiveText(&vt, "\x9b" "2J\x9bH\x9b" "12h\x9b?7h");
		sprintf(buf, "Connecting to %s on port %d\r\n", hostname, port);
		VT240ReceiveText(&vt, buf);

		TELNETInit(&telnet);
		telnet.rx = telnet_rx;
		vt.rx = telnet_tx;

		TELNETConnect(&telnet, argv[0], atoi(argv[1]));
	}

	// glutSetCursor(GLUT_CURSOR_NONE);

	time = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

	return 0;
}
