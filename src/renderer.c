#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <GL/gl.h>
#include <math.h>

#include "types.h"
#include "vt.h"
#include "vtfont.h"
#include "renderer.h"

#define	SCREEN_WIDTH		800
#define	SCREEN_HEIGHT		240

#define	BLUR_WIDTH		(SCREEN_WIDTH  * 1)
#define	BLUR_HEIGHT		(SCREEN_HEIGHT * 2)

#define	FONT_WIDTH		10
#define	FONT_HEIGHT		288

#define	TEXT_WIDTH		80
#define	TEXT_HEIGHT		24
#define	SETUP_TEXT_HEIGHT	8

#define	CURSOR_OFF_TIME		(2.0f / 3.0f)
#define	CURSOR_ON_TIME		(4.0f / 3.0f)
#define	CURSOR_TIME		(CURSOR_OFF_TIME + CURSOR_ON_TIME)

#define	BLINK_OFF_TIME		1.8f
#define	BLINK_ON_TIME		1.8f
#define	BLINK_TIME		(BLINK_OFF_TIME + BLINK_ON_TIME)

/* monochrome green */
#define	FGCOLOR_R_G	0.25f
#define	FGCOLOR_G_G	1.00f
#define	FGCOLOR_B_G	0.25f

/* monochrome white */
#define	FGCOLOR_R_W	1.00f
#define	FGCOLOR_G_W	1.00f
#define	FGCOLOR_B_W	1.00f

/* monochrome amber */
#define	FGCOLOR_R_A	1.00f
#define	FGCOLOR_G_A	0.658824f
#define	FGCOLOR_B_A	0.00f

extern const char vt240_vert[];
extern const char vt240_frag[];

extern const char post_vert[];
extern const char post_frag[];

extern const char blur_vert[];
extern const char blur_frag[];

#ifdef NDEBUG
#define GL_ERROR()
#else
extern void check_error(const char* filename, unsigned int line);
#define	GL_ERROR()	check_error(__FILE__, __LINE__)
#endif

static const float quad_vertices[] = {
	-1.0f, -1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,

	 1.0f,  1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f
};

#define	QUAD_VTX_CNT	(sizeof(quad_vertices) / (sizeof(*quad_vertices) * 3))

static const GLfloat vt240_colors[16][3] = {
	/* monochrome: green */
	/* color 0: black  */ { FGCOLOR_R_G * 0.00f, FGCOLOR_G_G * 0.00f, FGCOLOR_B_G * 0.00f },
	/* color 1: gray 1 */ { FGCOLOR_R_G * 0.30f, FGCOLOR_G_G * 0.30f, FGCOLOR_B_G * 0.30f },
	/* color 2: gray 2 */ { FGCOLOR_R_G * 0.50f, FGCOLOR_G_G * 0.50f, FGCOLOR_B_G * 0.50f },
	/* color 3: white  */ { FGCOLOR_R_G * 1.00f, FGCOLOR_G_G * 1.00f, FGCOLOR_B_G * 1.00f },

	/* monochrome: white */
	/* color 0: black  */ { FGCOLOR_R_W * 0.00f, FGCOLOR_G_W * 0.00f, FGCOLOR_B_W * 0.00f },
	/* color 1: gray 1 */ { FGCOLOR_R_W * 0.30f, FGCOLOR_G_W * 0.30f, FGCOLOR_B_W * 0.30f },
	/* color 2: gray 2 */ { FGCOLOR_R_W * 0.50f, FGCOLOR_G_W * 0.50f, FGCOLOR_B_W * 0.50f },
	/* color 3: white  */ { FGCOLOR_R_W * 1.00f, FGCOLOR_G_W * 1.00f, FGCOLOR_B_W * 1.00f },

	/* monochrome: amber */
	/* color 0: black  */ { FGCOLOR_R_A * 0.00f, FGCOLOR_G_A * 0.00f, FGCOLOR_B_A * 0.00f },
	/* color 1: gray 1 */ { FGCOLOR_R_A * 0.30f, FGCOLOR_G_A * 0.30f, FGCOLOR_B_A * 0.30f },
	/* color 2: gray 2 */ { FGCOLOR_R_A * 0.50f, FGCOLOR_G_A * 0.50f, FGCOLOR_B_A * 0.50f },
	/* color 3: white  */ { FGCOLOR_R_A * 1.00f, FGCOLOR_G_A * 1.00f, FGCOLOR_B_A * 1.00f },

	/* color */
	/* color 0: black  */ { 0.0f, 0.0f, 0.0f },
	/* color 1: blue   */ { 0.0f, 0.0f, 0.6f },
	/* color 2: red    */ { 0.6f, 0.0f, 0.0f },
	/* color 3: green  */ { 0.0f, 0.5f, 0.0f },
};

void VTInitRenderer(VTRenderer* self, VT240* vt)
{
	memset(self, 0, sizeof(VTRenderer));
	self->vt = vt;

	self->blink_time = 0;
	self->enable_glow = true;

	self->vt_shader = VTCreateShader(vt240_vert, vt240_frag);
	self->vt_shader_font_80 = glGetUniformLocation(self->vt_shader, "font_80");
	self->vt_shader_font_132 = glGetUniformLocation(self->vt_shader, "font_132");
	self->vt_shader_text = glGetUniformLocation(self->vt_shader, "text");
	self->vt_shader_line_attributes = glGetUniformLocation(self->vt_shader, "line_attributes");
	self->vt_shader_setup_text = glGetUniformLocation(self->vt_shader, "setup_text");
	self->vt_shader_setup_line_attributes = glGetUniformLocation(self->vt_shader, "setup_line_attributes");
	self->vt_shader_framebuffer = glGetUniformLocation(self->vt_shader, "framebuffer");
	self->vt_shader_text_size = glGetUniformLocation(self->vt_shader, "text_size");
	self->vt_shader_colorscheme = glGetUniformLocation(self->vt_shader, "colorscheme");
	self->vt_shader_cursor = glGetUniformLocation(self->vt_shader, "cursor");
	self->vt_shader_cursor_time = glGetUniformLocation(self->vt_shader, "cursor_time");
	self->vt_shader_blink_time = glGetUniformLocation(self->vt_shader, "blink_time");
	self->vt_shader_mode = glGetUniformLocation(self->vt_shader, "mode");
	self->vt_shader_in_setup = glGetUniformLocation(self->vt_shader, "in_setup");
	self->vt_shader_block_cursor = glGetUniformLocation(self->vt_shader, "block_cursor");

	self->blur_shader = VTCreateShader(blur_vert, blur_frag);
	self->blur_shader_tex = glGetUniformLocation(self->blur_shader, "image");
	self->blur_shader_dir = glGetUniformLocation(self->blur_shader, "direction");

	self->post_shader = VTCreateShader(post_vert, post_frag);
	self->post_shader_fbtex = glGetUniformLocation(self->post_shader, "vt240_screen");
	self->post_shader_blurtex = glGetUniformLocation(self->post_shader, "blur_texture");
	self->post_shader_enableglow = glGetUniformLocation(self->post_shader, "enable_glow");
	self->post_shader_raw = glGetUniformLocation(self->post_shader, "raw_mode");
	self->post_shader_focus = glGetUniformLocation(self->post_shader, "focus");
	self->post_shader_intensity = glGetUniformLocation(self->post_shader, "brightness");
	GL_ERROR();

	VTCreateBuffers(self);
	VTCreateFrameBuffer(self);
	VTCreateGraphicsTexture(self);
	VTCreateFontTexture(self);
	VTCreateTextTexture(self);

	VTCreateBlurFrameBuffer(self);
}

void VTEnableGlow(VTRenderer* self, bool enabled)
{
	self->enable_glow = enabled;
}

void VTSetRaw(VTRenderer* self, bool raw)
{
	self->raw = raw;
}

void VTSetFocus(VTRenderer* self, float focus)
{
	self->focus = focus;
}

void VTSetIntensity(VTRenderer* self, float intensity)
{
	self->intensity = intensity;
}

void VTProcess(VTRenderer* self, unsigned long dt)
{
	self->blink_time = (self->blink_time + dt)
		% (unsigned long) roundf(BLINK_TIME * 1000.0f);
}

void VTRender(VTRenderer* self, unsigned int width, unsigned int height)
{
	GL_ERROR();

	// PASS 0: render VT240 screen to texture
	VTRenderTerminal(self);

	GL_ERROR();

	// PASS 1/2/3/4: blur
	VTRenderBlur(self);

	GL_ERROR();

	// PASS 3: post-process VT240 screen
	glViewport(0, 0, width, height);
	glUseProgram(self->post_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->vt_tex);
	glUniform1i(self->post_shader_fbtex, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, self->blur_tex[1]);
	glUniform1i(self->post_shader_blurtex, 1);

	glUniform1i(self->post_shader_enableglow, self->enable_glow);
	glUniform1i(self->post_shader_raw, self->raw);
	glUniform1f(self->post_shader_focus, self->focus);
	glUniform1f(self->post_shader_intensity, self->intensity);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(self->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);

	GL_ERROR();
}

void VTRenderTerminal(VTRenderer* self)
{
	// PASS 0: render VT240 screen to texture
	glBindFramebuffer(GL_FRAMEBUFFER, self->vt_fb);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glUseProgram(self->vt_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->font_80_tex);
	glUniform1i(self->vt_shader_font_80, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, self->font_132_tex);
	glUniform1i(self->vt_shader_font_132, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, self->text_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, self->vt->columns, TEXT_HEIGHT, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, (GLvoid*) self->vt->text);
	glUniform1i(self->vt_shader_text, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, self->line_attrib_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, TEXT_HEIGHT, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, (GLvoid*) self->vt->line_attributes);
	glUniform1i(self->vt_shader_line_attributes, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, self->setup_text_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, self->vt->columns, SETUP_TEXT_HEIGHT, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, (GLvoid*) self->vt->setup.text);
	glUniform1i(self->vt_shader_setup_text, 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, self->setup_line_attrib_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, SETUP_TEXT_HEIGHT, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, (GLvoid*) self->vt->setup.line_attributes);
	glUniform1i(self->vt_shader_setup_line_attributes, 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, self->graphics_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) self->vt->framebuffer);
	glUniform1i(self->vt_shader_framebuffer, 6);

	glUniform2ui(self->vt_shader_text_size, self->vt->columns, self->vt->lines);
	glUniform2ui(self->vt_shader_cursor, self->vt->cursor_x, self->vt->cursor_y);
	glUniform1f(self->vt_shader_cursor_time, self->vt->cursor_time / 1000.0f);
	glUniform1f(self->vt_shader_blink_time, self->blink_time / 1000.0f);
	glUniform1ui(self->vt_shader_mode, self->vt->mode);
	glUniform1ui(self->vt_shader_in_setup, self->vt->in_setup);
	glUniform1ui(self->vt_shader_block_cursor, self->vt->config.cursor_style == VT240_CURSOR_STYLE_BLOCK_CURSOR);

	if(self->vt->config.display == VT240_DISPLAY_MONOCHROME) {
		glUniform3fv(self->vt_shader_colorscheme, 4, (GLfloat*) vt240_colors[4 * self->vt->screen_color]);
	} else {
		glUniform3fv(self->vt_shader_colorscheme, 4, (GLfloat*) &vt240_colors[12]);
	}

	glBindVertexArray(self->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);

	// unbind textures
	for(unsigned int i = 0; i < 7; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GL_ERROR();
}

void VTRenderBlur(VTRenderer* self)
{
	// PASS 0: render horizontal blur
	glBindFramebuffer(GL_FRAMEBUFFER, self->blur_fb[0]);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, BLUR_WIDTH, BLUR_HEIGHT);

	glUseProgram(self->blur_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->vt_tex);

	glUniform1i(self->blur_shader_tex, 0);
	glUniform2f(self->blur_shader_dir, 1.0f, 0.0f);

	glBindVertexArray(self->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);

	// PASS 1: render vertical blur
	glBindFramebuffer(GL_FRAMEBUFFER, self->blur_fb[1]);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, BLUR_WIDTH, BLUR_HEIGHT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->blur_tex[0]);

	glUniform2f(self->blur_shader_dir, 0.0f, 1.0f);

	glBindVertexArray(self->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);

	// PASS 2: render horizontal blur
	glBindFramebuffer(GL_FRAMEBUFFER, self->blur_fb[0]);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, BLUR_WIDTH, BLUR_HEIGHT);

	glUseProgram(self->blur_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->blur_tex[1]);

	glUniform1i(self->blur_shader_tex, 0);
	glUniform2f(self->blur_shader_dir, 1.0f, 0.0f);

	glBindVertexArray(self->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);

	// PASS 3: render vertical blur
	glBindFramebuffer(GL_FRAMEBUFFER, self->blur_fb[1]);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, BLUR_WIDTH, BLUR_HEIGHT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->blur_tex[0]);

	glUniform2f(self->blur_shader_dir, 0.0f, 1.0f);

	glBindVertexArray(self->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);

	// unbind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void VTCreateBuffers(VTRenderer* self)
{
	glGenVertexArrays(1, &self->quad_vao);
	glBindVertexArray(self->quad_vao);

	GLuint loc = 0;

	glGenBuffers(1, &self->quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self->quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc , 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void VTCreateGraphicsTexture(VTRenderer* self)
{
	glGenTextures(1, &self->graphics_tex);
	glBindTexture(GL_TEXTURE_2D, self->graphics_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_ERROR();
}

void VTCreateFontTexture(VTRenderer* self)
{
	glGenTextures(1, &self->font_80_tex);
	glBindTexture(GL_TEXTURE_2D, self->font_80_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, FONT_WIDTH, FONT_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, vt240_80colfont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_ERROR();

	glGenTextures(1, &self->font_132_tex);
	glBindTexture(GL_TEXTURE_2D, self->font_132_tex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, FONT_WIDTH, FONT_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, vt240_132colfont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_ERROR();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void VTCreateTextTexture(VTRenderer* self)
{
	// normal mode
	glGenTextures(1, &self->text_tex);
	glBindTexture(GL_TEXTURE_2D, self->text_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, TEXT_WIDTH, TEXT_HEIGHT, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &self->line_attrib_tex);
	glBindTexture(GL_TEXTURE_2D, self->line_attrib_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, TEXT_HEIGHT, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_ERROR();

	// setup mode
	glGenTextures(1, &self->setup_text_tex);
	glBindTexture(GL_TEXTURE_2D, self->setup_text_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, TEXT_WIDTH, SETUP_TEXT_HEIGHT, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &self->setup_line_attrib_tex);
	glBindTexture(GL_TEXTURE_2D, self->setup_line_attrib_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, SETUP_TEXT_HEIGHT, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_ERROR();
}

void VTCreateFrameBuffer(VTRenderer* self)
{
	glGenFramebuffers(1, &self->vt_fb);
	glGenTextures(1, &self->vt_tex);

	glBindTexture(GL_TEXTURE_2D, self->vt_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, self->vt_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->vt_tex, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("error configuring framebuffer\n");
		exit(1);
	}

	GL_ERROR();
}

void VTCreateBlurFrameBuffer(VTRenderer* self)
{
	glGenFramebuffers(2, self->blur_fb);
	glGenTextures(2, self->blur_tex);

	for(unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, self->blur_tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLUR_WIDTH, BLUR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, self->blur_fb[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->blur_tex[i], 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("error configuring framebuffer\n");
			exit(1);
		}
	}
}

GLuint VTCompileShader(GLuint type, const char* src)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, 0);
	glCompileShader(shader);

	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE) {
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(!len) {
			printf("Failed to retrieve shader compilation error log\n");
			exit(1);
		}

		char* log = (char*) malloc(len);
		glGetShaderInfoLog(shader, len, &len, log);
		glDeleteShader(shader);

		printf("Failed to compile shader:\n%s\n", log);
		free(log);

		exit(1);
	} else {
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(len) {
			char* log = (char*) malloc(len);
			glGetShaderInfoLog(shader, len, &len, log);
			// emscripten shit can't get this right
			if(len) {
				printf("shader compilation log:\n%s\n", log);
			}
			free(log);
		}
	}

	return shader;
}

GLuint VTCreateShader(const char* vs_src, const char* fs_src)
{
	GLuint vs = VTCompileShader(GL_VERTEX_SHADER, vs_src);
	GLuint fs = VTCompileShader(GL_FRAGMENT_SHADER, fs_src);

	GLuint shader = glCreateProgram();

	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	GLint linked = 0;
	glGetProgramiv(shader, GL_LINK_STATUS, &linked);
	if(linked == GL_FALSE) {
		GLint len = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(!len) {
			printf("Faild to retrieve shader linking error log\n");
			exit(1);
		}

		char* log = (char*) malloc(len);
		glGetProgramInfoLog(shader, len, &len, log);

		glDeleteProgram(shader);
		glDeleteShader(vs);
		glDeleteShader(fs);

		printf("Failed to link shader:\n%s\n", log);

		free(log);
		exit(1);
	} else {
		GLint len = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);

		if(len) {
			char* log = (char*) malloc(len);
			glGetProgramInfoLog(shader, len, &len, log);

			// emscripten shit can't get this right
			if(len) {
				printf("Shader linking error log:\n%s\n", log);
			}

			free(log);
		}
	}

	glDetachShader(shader, vs);
	glDetachShader(shader, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return shader;
}
