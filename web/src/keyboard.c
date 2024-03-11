#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "types.h"
#include "vt.h"

static unsigned long repeat_time = 0;
static unsigned int repeat_state = 0;
static unsigned int key_count = 0;
static u16 keys_down[384] = { 0 };

void VT240InitKeyboard(VT240* vt)
{
	repeat_time = 0;
	repeat_state = 0;
	key_count = 0;
	memset(keys_down, 0, 384 * sizeof(u16));
}

void VT240ProcessKeys(VT240* vt, unsigned long dt)
{
	int i, j;
	int count = 0;

	repeat_time += dt;

	if(!(vt->mode & DECARM)) {
		return;
	}

	if(key_count == 0) {
		return;
	}

	if(repeat_state == 0) {
		if(repeat_time < 500) {
			return;
		} else {
			repeat_time = 0;
			repeat_state = 1;
		}
	} else if(repeat_state == 1) {
		float cnt = (repeat_time * 30) / 1000.0f;
		count = (int) cnt;
		if(count > 0) {
			repeat_time -= count * (1000.0f / 30.0f);
		}
	}

	if(count > 0) {
		vt->cursor_time = 0;
	}

	for(j = 0; j < count; j++) {
		for(i = 0; i < key_count; i++) {
			u16 key = keys_down[i];
			VT240ProcessKey(vt, key);
		}
	}
}

void VT240KeyDown(VT240* vt, u16 key)
{
	if((repeat_state == 0 && repeat_time >= 500) || (repeat_state == 1)) {
		repeat_state = 0;
		key_count = 0;
		memset(keys_down, 0, 384 * sizeof(u16));
	}

	repeat_time = 0;
	repeat_state = 0;
	keys_down[key_count++] = key;

	vt->cursor_time = 0;

	VT240ProcessKey(vt, key);
}

void VT240KeyUp(VT240* vt, u16 key)
{
	int i;
	int found = 0;

	if(key_count == 0) {
		return;
	}

	for(i = 0; i < key_count; i++) {
		if(found) {
			keys_down[i - 1] = keys_down[i];
		} else if(keys_down[i] == key) {
			found = 1;
		}
	}

	if(found) {
		key_count--;
		repeat_time = 0;
		repeat_state = 0;
	}
}
