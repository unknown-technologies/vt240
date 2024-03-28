#include <emscripten/html5.h>

static EM_BOOL is_fullscreen_active = EM_FALSE;

void toggle_fullscreen_handler(void)
{
	if(is_fullscreen_active) {
		EMSCRIPTEN_RESULT result = emscripten_exit_fullscreen();

		if (result == EMSCRIPTEN_RESULT_SUCCESS) {
			is_fullscreen_active = EM_FALSE;
		}
	}else {
		EMSCRIPTEN_RESULT result = emscripten_request_fullscreen("#canvas", EM_FALSE);

		if (result == EMSCRIPTEN_RESULT_SUCCESS) {
			is_fullscreen_active = EM_TRUE;
		}
	}
}
