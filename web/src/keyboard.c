#include <string.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

#include "keyboardspecialhandler.h"
#include "types.h"
#include "vt.h"

#define KEY_MODIFIER_IGNORE	0x00
#define KEY_MODIFIER_NONE	0x01
#define KEY_MODIFIER_CTRL	0x02
#define KEY_MODIFIER_ALT	0x04
#define KEY_MODIFIER_META	0x08
#define KEY_MODIFIER_SHIFT	0x10

typedef struct {
	const char*	code;
	void		(*handler)(void);
	u16		vtKey;
	u8		modifiers;
} KeyMapEntry;

static const KeyMapEntry VT240KeyMap[] = {
	{.code = "Backspace",	.vtKey = CAN,	.modifiers = KEY_MODIFIER_CTRL},
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
	{.code = "F2",	.handler = toggle_fullscreen_handler,	.modifiers = KEY_MODIFIER_NONE},
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
	{.code = "Numpad0",		.vtKey = VT240_KEY_KP_0},
	{.code = "Numpad1",		.vtKey = VT240_KEY_KP_1},
	{.code = "Numpad2",		.vtKey = VT240_KEY_KP_2},
	{.code = "Numpad3",		.vtKey = VT240_KEY_KP_3},
	{.code = "Numpad4",		.vtKey = VT240_KEY_KP_4},
	{.code = "Numpad5",		.vtKey = VT240_KEY_KP_5},
	{.code = "Numpad6",		.vtKey = VT240_KEY_KP_6},
	{.code = "Numpad7",		.vtKey = VT240_KEY_KP_7},
	{.code = "Numpad8",		.vtKey = VT240_KEY_KP_8},
	{.code = "Numpad9",		.vtKey = VT240_KEY_KP_9},
	{.code = "NumpadEnter",		.vtKey = VT240_KEY_KP_ENTER},
	{.code = "NumpadAdd",		.vtKey = VT240_KEY_KP_MINUS},
	{.code = "NumpadSubtract", 	.vtKey = VT240_KEY_KP_PF4},
	{.code = "NumpadMultiply", 	.vtKey = VT240_KEY_KP_PF3},
	{.code = "NumpadDivide", 	.vtKey = VT240_KEY_KP_PF2},
	{.code = "NumpadDecimal", 	.vtKey = VT240_KEY_KP_PERIOD},
	{.code = "NumLock", 		.vtKey = VT240_KEY_KP_PF1},
};
static const int VT240KeyMapLength = sizeof(VT240KeyMap) / sizeof(KeyMapEntry);

static u8 get_modifier_bitmask(const EmscriptenKeyboardEvent *e)
{
	if(e->ctrlKey || e->altKey || e->metaKey || e->shiftKey) {
		return (u8) (
			(e->ctrlKey << 1) |
			(e->altKey << 2) |
			(e->metaKey << 3) |
			(e->shiftKey << 4)
		);
	}

	return KEY_MODIFIER_NONE;
}

static EM_BOOL handle_keyboard_input(int event_type, const EmscriptenKeyboardEvent *e, void* user_data)
{
	VT240* vt = (VT240*) user_data;

	if(e->repeat && !(vt->mode & DECARM)) {
		return EM_TRUE;
	}

	const KeyMapEntry* mapped_key = NULL;

	for(int i = 0; i < VT240KeyMapLength; i++) {
		if(
			strncmp(e->code, VT240KeyMap[i].code, 32) == 0 && (
				(VT240KeyMap[i].modifiers == KEY_MODIFIER_IGNORE) ||
				(VT240KeyMap[i].modifiers == get_modifier_bitmask(e))
			)
		) {
			mapped_key = &VT240KeyMap[i];
			break;
		}
	}

	if(mapped_key != NULL) {
		if (mapped_key->handler != NULL) {
			mapped_key->handler();
		}else {
			VT240ProcessKey(vt, mapped_key->vtKey);
		}
	} else if(strlen(e->key) == 1 && e->key[0] >= 0x20) {
		if (e->ctrlKey) {
			if(e->key[0] >= 'A' && e->key[0] <= 'Z') {
				VT240ProcessKey(vt, e->key[0] - 0x40);
			} else if(e->key[0] >= 'a' && e->key[0] <= 'z') {
				VT240ProcessKey(vt, e->key[0] - 0x60);
			}
		} else {
			VT240ProcessKey(vt, e->key[0]);
		}
	} else if(strlen(e->key) == 2) {
		u16 upper = (u16) (e->key[0] & 0x1F);
		u16 lower = (u16) (e->key[1] & 0x3F);
		u16 utf8_value = (upper << 6) + lower;

		switch(utf8_value) {
			case 0xA4:
				utf8_value = 0xA8;
				break;
			case 0x0152:
				utf8_value = 0xD7;
				break;
			case 0x0178:
				utf8_value = 0xDD;
				break;
			case 0x0153:
				utf8_value = 0xF7;
				break;
			case 0xFF:
				utf8_value = 0xFD;
				break;
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
				utf8_value = 0;
				break;
			default:
				break;
		}

		if(utf8_value > 0xA0 && utf8_value < 0xFF) {
			VT240ProcessKey(vt, utf8_value);
		}
	} else {
#ifndef NDEBUG
		printf("Found unmapped key, key: '%s', code: '%s', bitmask: '%x'\n", e->key, e->code, get_modifier_bitmask(e));
#endif
	}

	return EM_TRUE;
}

void VT240InitKeyboard(VT240* vt)
{
	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, vt, EM_TRUE, handle_keyboard_input);
}

void VT240ProcessKeys(VT240* vt, unsigned long dt)
{
	// Just a stub, not needed with Emscripten.
}
