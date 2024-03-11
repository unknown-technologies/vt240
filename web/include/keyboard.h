#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "vt.h"

void VT240InitKeyboard(VT240* vt);
void VT240KeyDown(VT240* vt, u16 key);
void VT240KeyUp(VT240* vt, u16 key);

#endif
