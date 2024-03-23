#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

#include "types.h"
#include "vt.h"

#define KEY_MODIFIER_IGNORE	0x00
#define KEY_MODIFIER_NONE	0x01
#define KEY_MODIFIER_CTRL	0x02
#define KEY_MODIFIER_ALT	0x04
#define KEY_MODIFIER_META	0x08
#define KEY_MODIFIER_SHIFT	0x10

struct KeyMapEntry {
	char* code;
	u16 vtKey;
	u8 modifiers;
};

static const struct KeyMapEntry VT240KeyMap[] = {
	{.code = "Backspace",	.vtKey = DEL},
	{.code = "Enter",	.vtKey = CR,	.modifiers = KEY_MODIFIER_NONE},
	{.code = "Escape",	.vtKey = ESC},
	{.code = "Tab",		.vtKey = HT},

	// ArrowKeys
	{.code = "ArrowUp",	.vtKey = VT240_KEY_UP},
	{.code = "ArrowDown",	.vtKey = VT240_KEY_DOWN},
	{.code = "ArrowLeft",	.vtKey = VT240_KEY_LEFT},
	{.code = "ArrowRight",	.vtKey = VT240_KEY_RIGHT},

	// special keys
	{.code = "Insert",	.vtKey = VT240_KEY_INSERT},
	{.code = "Delete",	.vtKey = VT240_KEY_REMOVE},
	{.code = "PageUp",	.vtKey = VT240_KEY_PREV_SCREEN},
	{.code = "PageDown",	.vtKey = VT240_KEY_NEXT_SCREEN},

	// F Keys
	{.code = "F1",	.vtKey = VT240_KEY_HOLD_SCREEN,		.modifiers = KEY_MODIFIER_NONE},
	{.code = "F2",	.vtKey = VT240_KEY_PRINT_SCREEN,	.modifiers = KEY_MODIFIER_NONE},
	{.code = "F3",	.vtKey = VT240_KEY_SET_UP,		.modifiers = KEY_MODIFIER_NONE},
	{.code = "F4",	.vtKey = VT240_KEY_DATA_TALK,		.modifiers = KEY_MODIFIER_NONE},
	{.code = "F5",	.vtKey = VT240_KEY_BREAK,		.modifiers = KEY_MODIFIER_NONE},
	{.code = "F6",	.vtKey = VT240_KEY_F6,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F7",	.vtKey = VT240_KEY_F7,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F8",	.vtKey = VT240_KEY_F8,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F9",	.vtKey = VT240_KEY_F9,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F10",	.vtKey = VT240_KEY_F10,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F11",	.vtKey = VT240_KEY_F11,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F12",	.vtKey = VT240_KEY_F12,			.modifiers = KEY_MODIFIER_NONE},
	{.code = "F3",	.vtKey = VT240_KEY_F13,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F4",	.vtKey = VT240_KEY_F14,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F5",	.vtKey = VT240_KEY_F15,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F6",	.vtKey = VT240_KEY_F16,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F7",	.vtKey = VT240_KEY_F17,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F8",	.vtKey = VT240_KEY_F18,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F9",	.vtKey = VT240_KEY_F19,			.modifiers = KEY_MODIFIER_ALT},
	{.code = "F10",	.vtKey = VT240_KEY_F20,			.modifiers = KEY_MODIFIER_ALT},

	// NumPad
	{.code = "NumPad0",		.vtKey = VT240_KEY_KP_0},
	{.code = "NumPad1",		.vtKey = VT240_KEY_KP_1},
	{.code = "NumPad2",		.vtKey = VT240_KEY_KP_2},
	{.code = "NumPad3",		.vtKey = VT240_KEY_KP_3},
	{.code = "NumPad4",		.vtKey = VT240_KEY_KP_4},
	{.code = "NumPad5",		.vtKey = VT240_KEY_KP_5},
	{.code = "NumPad6",		.vtKey = VT240_KEY_KP_6},
	{.code = "NumPad7",		.vtKey = VT240_KEY_KP_7},
	{.code = "NumPad8",		.vtKey = VT240_KEY_KP_8},
	{.code = "NumPad9",		.vtKey = VT240_KEY_KP_9},
	{.code = "NumpadEnter",		.vtKey = VT240_KEY_KP_ENTER},
	{.code = "NumpadAdd",		.vtKey = VT240_KEY_KP_MINUS},
	{.code = "NumpadSubtract", 	.vtKey = VT240_KEY_KP_PF4},
	{.code = "NumpadMultiply", 	.vtKey = VT240_KEY_KP_PF3},
	{.code = "NumpadDivide", 	.vtKey = VT240_KEY_KP_PF2},
	{.code = "NumpadDecimal", 	.vtKey = VT240_KEY_KP_PERIOD},
	{.code = "NumLock", 		.vtKey = VT240_KEY_KP_PF1},
};
static const int VT240KeyMapLength = sizeof(VT240KeyMap) / sizeof(struct KeyMapEntry);

u8 getModifierBitmask(const EmscriptenKeyboardEvent *e) {
	if (e->ctrlKey || e->altKey || e->metaKey || e->shiftKey) {
		return (u8) (
			(e->ctrlKey << 1) |
			(e->altKey << 2) |
			(e->metaKey << 3) |
			(e->shiftKey << 4)
		);
	}

	return KEY_MODIFIER_NONE;
}

EM_BOOL handleKeyboardInput(int eventType, const EmscriptenKeyboardEvent *e, void* userData)
{
	// first cast the userData to the actual vt we want to use.
	VT240* vt = (VT240*) userData;

	// if repeat is disabled, but this is an repeat event, just skip it.
	if (e->repeat && !(vt->mode & DECARM)) {
		return EM_TRUE;
	}
	// else we check, whether the key is a special key with a mapping

	const struct KeyMapEntry* mappedKey = NULL;

	for (int i = 0; i < VT240KeyMapLength; i++) {
		if (
			strncmp(e->code, VT240KeyMap[i].code, 32) == 0 && (
				(VT240KeyMap[i].modifiers == KEY_MODIFIER_IGNORE) ||
				(VT240KeyMap[i].modifiers == getModifierBitmask(e))
			)
		) {
			mappedKey = &VT240KeyMap[i];

			break;
		}
	}

	// if we found any key, let's send it to the vt
	if (mappedKey != NULL) {
		VT240ProcessKey(vt, mappedKey->vtKey);
	}
	// else we check, whether it's a printable character
	else if (strlen(e->key) == 1 && e->key[0] >= 0x20) {
		if (e->ctrlKey) {
			if (e->key[0] >= 'a' && e->key[0] <= 'z') {
				VT240ProcessKey(vt, e->key[0] - 0x60);
			}
			else if (e->key[0] >= 'A' && e->key[0] <= 'Z') {
				VT240ProcessKey(vt, e->key[0] - 0x40);
			}
		}
		else {
			VT240ProcessKey(vt, e->key[0]);
		}
	}
	else if (strlen(e->key) == 2) {
		u16 upper = (u16) (e->key[0] & 0x1F);
		u16 lower = (u16) (e->key[1] & 0x3F);
		u16 utf8Value = (upper << 6) + lower;

		switch (utf8Value) {
			case 0xA4:
				utf8Value = 0xA8;
				break;
			case 0xFF:
				utf8Value = 0xFD;
				break;
			// these are control characters, so we filter them
			case 0xA6:
			case 0xAC:
			case 0xAD:
			case 0xAE:
			case 0xAF:
			case 0xB4:
			case 0xB8:
			case 0xBE:
			case 0xD0:
			case 0xDE:
			case 0xF0:
			case 0xFE:
				utf8Value = 0;
				break;
			default:
				// the utf8 value is just right
				break;
		}

		if (utf8Value > 0xA0 && utf8Value < 0xFF) {
			VT240ProcessKey(vt, utf8Value);
		}
	}
	// else it's an unmapped key, so just log it.
	else {
		printf("Found unmapped key, key: '%s', code: '%s', bitmask: '%x'\n", e->key, e->code, getModifierBitmask(e));
	}

	// return true as "preventDefault"
	return EM_TRUE;
}

void VT240InitKeyboard(VT240* vt)
{
	// we're passing vt as "void* userData" to the handleKeyboardInput.
	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, vt, EM_TRUE, handleKeyboardInput);
}

void VT240ProcessKeys(VT240* vt, unsigned long dt)
{
	// Just a stub, not needed with Emscripten.
}
