#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "vt.h"
#include <emscripten/html5.h>

void VT240InitKeyboard(VT240* vt);
EM_BOOL handleKeyboardInput(int eventType, const EmscriptenKeyboardEvent *e, void* userData);

#endif
