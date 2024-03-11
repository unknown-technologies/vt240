#include <GLFW/glfw3.h>

#include "types.h"
#include "vt.h"

/* TODO: keep track of all pressed keys and handle key up + autorepeat
 * appropriately */

void VT240InitKeyboard(VT240* vt)
{
	vt->modifiers = 0;
	vt->last_scancode = 0;
	vt->repeat_scancode = 0;
	vt->repeat_char = 0xFFFF;

	vt->repeat_time = 0;
	vt->repeat_state = 0;
}

u16 VT240TranslateKey(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);
	bool ctrl = (vt->modifiers & VT240_MODIFIER_CTRL_L) || (vt->modifiers & VT240_MODIFIER_CTRL_R);
	bool alt = (vt->modifiers & VT240_MODIFIER_ALT_L) || (vt->modifiers & VT240_MODIFIER_ALT_R);

	switch(key) {
		case GLFW_KEY_F1:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F11_UDK : VT240_KEY_F11;
			} else {
				return VT240_KEY_HOLD_SCREEN;
			}
		case GLFW_KEY_F2:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F12_UDK : VT240_KEY_F12;
			} else {
				return VT240_KEY_PRINT_SCREEN;
			}
		case GLFW_KEY_F3:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F13_UDK : VT240_KEY_F13;
			} else {
				return VT240_KEY_SET_UP;
			}
		case GLFW_KEY_F4:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F14_UDK : VT240_KEY_F14;
			} else {
				return VT240_KEY_DATA_TALK;
			}
		case GLFW_KEY_F5:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F15_UDK : VT240_KEY_F15;
			} else {
				return VT240_KEY_BREAK;
			}
		case GLFW_KEY_F6:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F16_UDK : VT240_KEY_F16;
			} else {
				return shift ? VT240_KEY_F6_UDK : VT240_KEY_F6;
			}
		case GLFW_KEY_F7:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F17_UDK : VT240_KEY_F17;
			} else {
				return shift ? VT240_KEY_F7_UDK : VT240_KEY_F7;
			}
		case GLFW_KEY_F8:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F18_UDK : VT240_KEY_F18;
			} else {
				return shift ? VT240_KEY_F8_UDK : VT240_KEY_F8;
			}
		case GLFW_KEY_F9:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F19_UDK : VT240_KEY_F19;
			} else {
				return shift ? VT240_KEY_F9_UDK : VT240_KEY_F9;
			}
		case GLFW_KEY_F10:
			if(ctrl || alt) {
				return shift ? VT240_KEY_F20_UDK : VT240_KEY_F20;
			} else {
				return shift ? VT240_KEY_F10_UDK : VT240_KEY_F10;
			}
		case GLFW_KEY_F11:
			return shift ? VT240_KEY_F11_UDK : VT240_KEY_F11;
		case GLFW_KEY_F12:
			return shift ? VT240_KEY_F12_UDK : VT240_KEY_F12;
		case GLFW_KEY_F13:
			return shift ? VT240_KEY_F13_UDK : VT240_KEY_F13;
		case GLFW_KEY_F14:
			return shift ? VT240_KEY_F14_UDK : VT240_KEY_F14;
		case GLFW_KEY_F15:
			return shift ? VT240_KEY_F15_UDK : VT240_KEY_F15;
		case GLFW_KEY_F16:
			return shift ? VT240_KEY_F16_UDK : VT240_KEY_F16;
		case GLFW_KEY_F17:
			return shift ? VT240_KEY_F17_UDK : VT240_KEY_F17;
		case GLFW_KEY_F18:
			return shift ? VT240_KEY_F18_UDK : VT240_KEY_F18;
		case GLFW_KEY_F19:
			return shift ? VT240_KEY_F19_UDK : VT240_KEY_F19;
		case GLFW_KEY_F20:
			return shift ? VT240_KEY_F20_UDK : VT240_KEY_F20;
		case GLFW_KEY_LEFT:
			return VT240_KEY_LEFT;
		case GLFW_KEY_UP:
			return VT240_KEY_UP;
		case GLFW_KEY_RIGHT:
			return VT240_KEY_RIGHT;
		case GLFW_KEY_DOWN:
			return VT240_KEY_DOWN;
		case GLFW_KEY_PAGE_UP:
			return VT240_KEY_PREV_SCREEN;
		case GLFW_KEY_PAGE_DOWN:
			return VT240_KEY_NEXT_SCREEN;
		case GLFW_KEY_HOME:
			return VT240_KEY_FIND;
		case GLFW_KEY_END:
			return VT240_KEY_SELECT;
		case GLFW_KEY_INSERT:
			return VT240_KEY_INSERT;
		case GLFW_KEY_DELETE:
			return VT240_KEY_REMOVE;
		case GLFW_KEY_BACKSPACE:
			return DEL;
		case GLFW_KEY_TAB:
			return HT;
		case GLFW_KEY_ESCAPE:
			return ESC;
		case GLFW_KEY_SPACE:
			return 0x20;
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:
			return VT240_KEY_SHIFT;
		case GLFW_KEY_ENTER:
			return CR;
		/* keypad */
		case GLFW_KEY_KP_0:
			return VT240_KEY_KP_0;
		case GLFW_KEY_KP_1:
			return VT240_KEY_KP_1;
		case GLFW_KEY_KP_2:
			return VT240_KEY_KP_2;
		case GLFW_KEY_KP_3:
			return VT240_KEY_KP_3;
		case GLFW_KEY_KP_4:
			return VT240_KEY_KP_4;
		case GLFW_KEY_KP_5:
			return VT240_KEY_KP_5;
		case GLFW_KEY_KP_6:
			return VT240_KEY_KP_6;
		case GLFW_KEY_KP_7:
			return VT240_KEY_KP_7;
		case GLFW_KEY_KP_8:
			return VT240_KEY_KP_8;
		case GLFW_KEY_KP_9:
			return VT240_KEY_KP_9;
		case GLFW_KEY_KP_DECIMAL:
			return VT240_KEY_KP_PERIOD;
		case GLFW_KEY_NUM_LOCK:
			return VT240_KEY_KP_PF1;
		case GLFW_KEY_KP_DIVIDE:
			return VT240_KEY_KP_PF2;
		case GLFW_KEY_KP_MULTIPLY:
			return VT240_KEY_KP_PF3;
		case GLFW_KEY_KP_SUBTRACT:
			return VT240_KEY_KP_PF4;
		case GLFW_KEY_KP_ADD:
			return VT240_KEY_KP_MINUS;
		case GLFW_KEY_KP_ENTER:
			return VT240_KEY_KP_ENTER;
		case GLFW_KEY_KP_EQUAL:
		default:
			return 0xFFFF;
	}
}

void VT240KeyboardKeyDownHost(VT240* vt, int key)
{
	bool ctrl = (vt->modifiers & VT240_MODIFIER_CTRL_L) || (vt->modifiers & VT240_MODIFIER_CTRL_R);

	switch(key) {
		case GLFW_KEY_LEFT_SHIFT:
			vt->modifiers |= VT240_MODIFIER_SHIFT_L;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			vt->modifiers |= VT240_MODIFIER_SHIFT_R;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			vt->modifiers |= VT240_MODIFIER_CTRL_L;
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			vt->modifiers |= VT240_MODIFIER_CTRL_R;
			break;
		case GLFW_KEY_LEFT_ALT:
			vt->modifiers |= VT240_MODIFIER_ALT_L;
			break;
		case GLFW_KEY_RIGHT_ALT:
			vt->modifiers |= VT240_MODIFIER_ALT_R;
			break;
		default:
			if(ctrl) {
				const char* name = glfwGetKeyName(key, 0);
				if(name && !(*name & 0x80) && !name[1]) {
					u16 value = *name;

					if((value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z')) {
						value &= ~0x20; // upper case

						u16 code = value - 'A' + 1;

						vt->repeat_time = 0;
						vt->repeat_state = 0;

						vt->repeat_scancode = key;
						vt->repeat_char = code;

						VT240ProcessKey(vt, code);
					} else {
						u8 code = 0xFF;

						switch(value) {
							case '2':
							case ' ':
								code = NUL;
								break;
							case '3':
							case '[':
								code = ESC;
								break;
							case '4':
							case '\\':
								code = FS;
								break;
							case '5':
							case ']':
								code = GS;
								break;
							case '6':
							case '~':
								code = RS;
								break;
							case '7':
							case '?':
								code = US;
								break;
							case '8':
								code = DEL;
								break;
						}

						if(code != 0xFF) {
							vt->repeat_time = 0;
							vt->repeat_state = 0;

							vt->repeat_scancode = key;
							vt->repeat_char = code;

							VT240ProcessKey(vt, code);
						} else {
							/* no code defined, use default ones */
							u16 code = VT240TranslateKey(vt, key);
							if(code == VT240_KEY_SET_UP) {
								VT240ProcessKey(vt, code);
							} else if(code != 0xFFFF) {
								vt->repeat_time = 0;
								vt->repeat_state = 0;

								vt->repeat_scancode = key;
								vt->repeat_char = 0xFFFF;

								VT240ProcessKey(vt, code);
							} else {
								vt->last_scancode = key;
							}
						}
					}
				} else if(key == GLFW_KEY_SPACE) {
					vt->repeat_time = 0;
					vt->repeat_state = 0;

					vt->repeat_scancode = key;
					vt->repeat_char = 0;

					VT240ProcessKey(vt, 0);
				}
			} else {
				u16 code = VT240TranslateKey(vt, key);
				if(code == VT240_KEY_SET_UP) {
					VT240ProcessKey(vt, code);
				} else if(code != 0xFFFF) {
					vt->repeat_time = 0;
					vt->repeat_state = 0;

					vt->repeat_scancode = key;
					vt->repeat_char = 0xFFFF;

					VT240ProcessKey(vt, code);
				} else {
					vt->last_scancode = key;
				}
			}
			break;
	}
}

void VT240KeyboardKeyUpHost(VT240* vt, int key)
{
	switch(key) {
		case GLFW_KEY_LEFT_SHIFT:
			vt->modifiers &= ~VT240_MODIFIER_SHIFT_L;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			vt->modifiers &= ~VT240_MODIFIER_SHIFT_R;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			vt->modifiers &= ~VT240_MODIFIER_CTRL_L;
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			vt->modifiers &= ~VT240_MODIFIER_CTRL_R;
			break;
		case GLFW_KEY_LEFT_ALT:
			vt->modifiers &= ~VT240_MODIFIER_ALT_L;
			break;
		case GLFW_KEY_RIGHT_ALT:
			vt->modifiers &= ~VT240_MODIFIER_ALT_R;
			break;
		default:
			if(vt->repeat_char < 256 && key == vt->repeat_scancode) {
				vt->repeat_char = 0xFFFF;
				vt->repeat_scancode = 0;
			} else if(vt->repeat_scancode == key) {
				vt->repeat_scancode = 0;
			}
			break;
	}
}

void VT240KeyboardChar(VT240* vt, unsigned int code)
{
	if(vt->config.keyboard != VT240_KEYBOARD_UNKNOWN) {
		return;
	}

	if(vt->last_scancode) {
		vt->repeat_scancode = vt->last_scancode;
		vt->repeat_char = code;
		vt->last_scancode = 0;

		vt->repeat_time = 0;
		vt->repeat_state = 0;

		if(code < 256) {
			VT240ProcessKey(vt, code);
		}
	}
}

#define	REPEAT_TIME	(1000 / 30)
void VT240ProcessKeys(VT240* vt, unsigned long dt)
{
	vt->repeat_time += dt;
	if(vt->repeat_state == 0 && vt->repeat_time >= 500) {
		vt->repeat_state = 1;
		vt->repeat_time -= 500;
	}
	if(vt->repeat_state == 1 && vt->repeat_time >= REPEAT_TIME) {
		if(vt->repeat_char < 256) {
			if(vt->mode & DECARM) {
				VT240ProcessKey(vt, vt->repeat_char);
			}
		} else if(vt->repeat_scancode) {
			u16 code = VT240TranslateKey(vt, vt->repeat_scancode);

			/* The following keys never repeat: Hold Screen, Print
			 * Screen, Set-Up, Data/Talk, Break, Return, Compose
			 * Character, Lock, Shift, and Ctrl. */
			switch(code) {
				case VT240_KEY_HOLD_SCREEN:
				case VT240_KEY_PRINT_SCREEN:
				case VT240_KEY_SET_UP:
				case VT240_KEY_DATA_TALK:
				case VT240_KEY_BREAK:
				case VT240_KEY_SHIFT:
				case CR:
					break;
				default:
					if(vt->mode & DECARM) {
						VT240ProcessKey(vt, code);
					}
			}
		}

		if(vt->repeat_time > 2 * REPEAT_TIME) {
			vt->repeat_time = 0;
		} else {
			vt->repeat_time -= REPEAT_TIME;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// North American Keyboard
u16 VT240KeyboardTranslateKey1(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? '~' : '`';
		case GLFW_KEY_1:
			return shift ? '!' : '1';
		case GLFW_KEY_2:
			return shift ? '@' : '2';
		case GLFW_KEY_3:
			return shift ? '#' : '3';
		case GLFW_KEY_4:
			return shift ? '$' : '4';
		case GLFW_KEY_5:
			return shift ? '%' : '5';
		case GLFW_KEY_6:
			return shift ? '^' : '6';
		case GLFW_KEY_7:
			return shift ? '&' : '7';
		case GLFW_KEY_8:
			return shift ? '*' : '8';
		case GLFW_KEY_9:
			return shift ? '(' : '9';
		case GLFW_KEY_0:
			return shift ? ')' : '0';
		case GLFW_KEY_MINUS:
			return shift ? '_' : '-';
		case GLFW_KEY_EQUAL:
			return shift ? '+' : '=';
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_W:
			return shift ? 'W' : 'w';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? '{' : '[';
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? '}' : ']';
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'A' : 'a';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? ':' : ';';
		case GLFW_KEY_APOSTROPHE:
			return shift ? '"' : '\'';
		case GLFW_KEY_BACKSLASH:
			return shift ? '|' : '\\';
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? 'M' : 'm';
		case GLFW_KEY_COMMA:
			return ',';
		case GLFW_KEY_PERIOD:
			return '.';
		case GLFW_KEY_SLASH:
			return shift ? '?' : '/';
		default:
			return VT240TranslateKey(vt, key);
	}
}

// British Keyboard
u16 VT240KeyboardTranslateKey2(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? 0xB0 : '~';
		case GLFW_KEY_1:
			return shift ? '!' : '1';
		case GLFW_KEY_2:
			return shift ? '"' : '2';
		case GLFW_KEY_3:
			return shift ? 0xA3 : '3';
		case GLFW_KEY_4:
			return shift ? '$' : '4';
		case GLFW_KEY_5:
			return shift ? '%' : '5';
		case GLFW_KEY_6:
			return shift ? '&' : '6';
		case GLFW_KEY_7:
			return shift ? '\'' : '7';
		case GLFW_KEY_8:
			return shift ? '(' : '8';
		case GLFW_KEY_9:
			return shift ? ')' : '9';
		case GLFW_KEY_0:
			return shift ? '=' : '0';
		case GLFW_KEY_MINUS:
			return shift ? '_' : '-';
		case GLFW_KEY_EQUAL:
			return shift ? 0xBD : 0xBC;
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_W:
			return shift ? 'W' : 'w';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? 0xA7 : '@';
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? ']' : '[';
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'A' : 'a';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? '+' : ';';
		case GLFW_KEY_APOSTROPHE:
			return shift ? '*' : ':';
		case GLFW_KEY_BACKSLASH:
			return shift ? '`' : '^';
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? 'M' : 'm';
		case GLFW_KEY_COMMA:
			return ',';
		case GLFW_KEY_PERIOD:
			return '.';
		case GLFW_KEY_SLASH:
			return shift ? '?' : '/';
		default:
			return VT240TranslateKey(vt, key);
	}
}

// Flemish Keyboard
// TODO: check for correctness of row 0
u16 VT240KeyboardTranslateKey3(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? '~' : '`';
		case GLFW_KEY_1:
			return shift ? '1' : '&';
		case GLFW_KEY_2:
			return shift ? '2' : 0xE9;
		case GLFW_KEY_3:
			return shift ? '3' : '"';
		case GLFW_KEY_4:
			return shift ? '4' : '\'';
		case GLFW_KEY_5:
			return shift ? '5' : '(';
		case GLFW_KEY_6:
			return shift ? '6' : 0xA7;
		case GLFW_KEY_7:
			return shift ? '7' : 0xE8;
		case GLFW_KEY_8:
			return shift ? '8' : '!';
		case GLFW_KEY_9:
			return shift ? '9' : 0xE7;
		case GLFW_KEY_0:
			return shift ? '0' : 0xE0;
		case GLFW_KEY_MINUS:
			return shift ? ')' : 0xB0;
		case GLFW_KEY_EQUAL:
			return shift ? '_' : '-';
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'A' : 'a';
		case GLFW_KEY_W:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? 0xFFFF : '^'; // TODO: dead key
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? '*' : '$';
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? 'M' : 'm';
		case GLFW_KEY_APOSTROPHE:
			return shift ? '%' : 0xD9;
		case GLFW_KEY_BACKSLASH:
			return shift ? '@' : '#';
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'W' : 'w';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? '?' : ',';
		case GLFW_KEY_COMMA:
			return shift ? '.' : ';';
		case GLFW_KEY_PERIOD:
			return shift ? '/' : ':';
		case GLFW_KEY_SLASH:
			return shift ? '+' : '=';
		default:
			return VT240TranslateKey(vt, key);
	}
}

// Canadian (French) Keyboard
u16 VT240KeyboardTranslateKey4(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? 0xB0 : '~';
		case GLFW_KEY_1:
			return shift ? '!' : '1';
		case GLFW_KEY_2:
			return shift ? '"' : '2';
		case GLFW_KEY_3:
			return shift ? '/' : '3';
		case GLFW_KEY_4:
			return shift ? '$' : '4';
		case GLFW_KEY_5:
			return shift ? '%' : '5';
		case GLFW_KEY_6:
			return shift ? '?' : '6';
		case GLFW_KEY_7:
			return shift ? '&' : '7';
		case GLFW_KEY_8:
			return shift ? '*' : '8';
		case GLFW_KEY_9:
			return shift ? '(' : '9';
		case GLFW_KEY_0:
			return shift ? ')' : '0';
		case GLFW_KEY_MINUS:
			return shift ? '_' : '-';
		case GLFW_KEY_EQUAL:
			return shift ? '+' : '=';
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_W:
			return shift ? 'W' : 'w';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? 0xC7 : 0xE7;
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? '@' : '#';
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'A' : 'a';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? ':' : ';';
		case GLFW_KEY_APOSTROPHE:
			return shift ? '^' : '`'; // TODO: dead key
		case GLFW_KEY_BACKSLASH:
			return shift ? '|' : '\\';
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? 'M' : 'm';
		case GLFW_KEY_COMMA:
			return ','; // TODO: dead key
		case GLFW_KEY_PERIOD:
			return '.';
		case GLFW_KEY_SLASH:
			return shift ? 0xC9 : 0xE9;
		default:
			return VT240TranslateKey(vt, key);
	}
}

// Danish Keyboard
u16 VT240KeyboardTranslateKey5(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? 0xB0 : '~';
		case GLFW_KEY_1:
			return shift ? '!' : '1';
		case GLFW_KEY_2:
			return shift ? '"' : '2';
		case GLFW_KEY_3:
			return shift ? 0xA7 : '3';
		case GLFW_KEY_4:
			return shift ? '$' : '4';
		case GLFW_KEY_5:
			return shift ? '%' : '5';
		case GLFW_KEY_6:
			return shift ? '&' : '6';
		case GLFW_KEY_7:
			return shift ? '/' : '7';
		case GLFW_KEY_8:
			return shift ? '(' : '8';
		case GLFW_KEY_9:
			return shift ? ')' : '9';
		case GLFW_KEY_0:
			return shift ? '=' : '0';
		case GLFW_KEY_MINUS:
			return shift ? '?' : '+';
		case GLFW_KEY_EQUAL:
			return shift ? '`' : 0xFFFF; // TODO: dead key
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_W:
			return shift ? 'W' : 'w';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? 0xC5 : 0xE5;
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? '^' : 0xFFFF; // TODO: dead key
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'A' : 'a';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? 0xC6 : 0xE6;
		case GLFW_KEY_APOSTROPHE:
			return shift ? 0xD8 : 0xE8;
		case GLFW_KEY_BACKSLASH:
			return shift ? '*' : 0xFFFF; // TODO: dead key
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? 'M' : 'm';
		case GLFW_KEY_COMMA:
			return shift ? ';' : ',';
		case GLFW_KEY_PERIOD:
			return shift ? ':' : '.';
		case GLFW_KEY_SLASH:
			return shift ? '_' : '-';
		default:
			return VT240TranslateKey(vt, key);
	}
}

// Finnish Keyboard
u16 VT240KeyboardTranslateKey6(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? 0xB0 : '~';
		case GLFW_KEY_1:
			return shift ? '!' : '1';
		case GLFW_KEY_2:
			return shift ? '"' : '2';
		case GLFW_KEY_3:
			return shift ? 0xA7 : '3';
		case GLFW_KEY_4:
			return shift ? '$' : '4';
		case GLFW_KEY_5:
			return shift ? '%' : '5';
		case GLFW_KEY_6:
			return shift ? '&' : '6';
		case GLFW_KEY_7:
			return shift ? '/' : '7';
		case GLFW_KEY_8:
			return shift ? '(' : '8';
		case GLFW_KEY_9:
			return shift ? ')' : '9';
		case GLFW_KEY_0:
			return shift ? '=' : '0';
		case GLFW_KEY_MINUS:
			return shift ? '?' : '+';
		case GLFW_KEY_EQUAL:
			return shift ? '`' : '^'; // TODO: dead key
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_W:
			return shift ? 'W' : 'w';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? 0xC5 : 0xE5;
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? 0xDC : 0xFC;
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'A' : 'a';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? 0xD6 : 0xF6;
		case GLFW_KEY_APOSTROPHE:
			return shift ? 0xC4 : 0xE4;
		case GLFW_KEY_BACKSLASH:
			return shift ? '*' : 0xFFFF; // TODO: dead key
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? 'M' : 'm';
		case GLFW_KEY_COMMA:
			return shift ? ';' : ',';
		case GLFW_KEY_PERIOD:
			return shift ? ':' : '.';
		case GLFW_KEY_SLASH:
			return shift ? '_' : '-';
		default:
			return VT240TranslateKey(vt, key);
	}
}

// German Keyboard
u16 VT240KeyboardTranslateKey7(VT240* vt, int key)
{
	bool shift = (vt->modifiers & VT240_MODIFIER_SHIFT_L) || (vt->modifiers & VT240_MODIFIER_SHIFT_R);

	switch(key) {
		/* Row 0 */
		case GLFW_KEY_GRAVE_ACCENT:
			return shift ? '^' : '~';
		case GLFW_KEY_1:
			return shift ? '!' : '1';
		case GLFW_KEY_2:
			return shift ? '"' : '2';
		case GLFW_KEY_3:
			return shift ? 0xA7 : '3';
		case GLFW_KEY_4:
			return shift ? '$' : '4';
		case GLFW_KEY_5:
			return shift ? '%' : '5';
		case GLFW_KEY_6:
			return shift ? '&' : '6';
		case GLFW_KEY_7:
			return shift ? '/' : '7';
		case GLFW_KEY_8:
			return shift ? '(' : '8';
		case GLFW_KEY_9:
			return shift ? ')' : '9';
		case GLFW_KEY_0:
			return shift ? '=' : '0';
		case GLFW_KEY_MINUS:
			return shift ? '?' : 0xDF;
		case GLFW_KEY_EQUAL:
			return shift ? '`' : 0xFFFF; // TODO: dead key
		/* Row 1 */
		case GLFW_KEY_Q:
			return shift ? 'Q' : 'q';
		case GLFW_KEY_W:
			return shift ? 'W' : 'w';
		case GLFW_KEY_E:
			return shift ? 'E' : 'e';
		case GLFW_KEY_R:
			return shift ? 'R' : 'r';
		case GLFW_KEY_T:
			return shift ? 'T' : 't';
		case GLFW_KEY_Y:
			return shift ? 'Z' : 'z';
		case GLFW_KEY_U:
			return shift ? 'U' : 'u';
		case GLFW_KEY_I:
			return shift ? 'I' : 'i';
		case GLFW_KEY_O:
			return shift ? 'O' : 'o';
		case GLFW_KEY_P:
			return shift ? 'P' : 'p';
		case GLFW_KEY_LEFT_BRACKET:
			return shift ? 0xDC : 0xFC;
		case GLFW_KEY_RIGHT_BRACKET:
			return shift ? '*' : '+';
		/* Row 2 */
		case GLFW_KEY_A:
			return shift ? 'A' : 'a';
		case GLFW_KEY_S:
			return shift ? 'S' : 's';
		case GLFW_KEY_D:
			return shift ? 'D' : 'd';
		case GLFW_KEY_F:
			return shift ? 'F' : 'f';
		case GLFW_KEY_G:
			return shift ? 'G' : 'g';
		case GLFW_KEY_H:
			return shift ? 'H' : 'h';
		case GLFW_KEY_J:
			return shift ? 'J' : 'j';
		case GLFW_KEY_K:
			return shift ? 'K' : 'k';
		case GLFW_KEY_L:
			return shift ? 'L' : 'l';
		case GLFW_KEY_SEMICOLON:
			return shift ? 0xD6 : 0xF6;
		case GLFW_KEY_APOSTROPHE:
			return shift ? 0xC4 : 0xE4;
		case GLFW_KEY_BACKSLASH:
			return shift ? '\'' : '#';
		/* Row 3 */
		case GLFW_KEY_WORLD_1:
			return shift ? '>' : '<';
		case GLFW_KEY_Z:
			return shift ? 'Y' : 'y';
		case GLFW_KEY_X:
			return shift ? 'X' : 'x';
		case GLFW_KEY_C:
			return shift ? 'C' : 'c';
		case GLFW_KEY_V:
			return shift ? 'V' : 'v';
		case GLFW_KEY_B:
			return shift ? 'B' : 'b';
		case GLFW_KEY_N:
			return shift ? 'N' : 'n';
		case GLFW_KEY_M:
			return shift ? 'M' : 'm';
		case GLFW_KEY_COMMA:
			return shift ? ';' : ',';
		case GLFW_KEY_PERIOD:
			return shift ? ':' : '.';
		case GLFW_KEY_SLASH:
			return shift ? '_' : '-';
		default:
			return VT240TranslateKey(vt, key);
	}
}

void VT240KeyboardKeyDownLocal(VT240* vt, int key)
{
	bool ctrl = (vt->modifiers & VT240_MODIFIER_CTRL_L) || (vt->modifiers & VT240_MODIFIER_CTRL_R);

	switch(key) {
		case GLFW_KEY_LEFT_SHIFT:
			vt->modifiers |= VT240_MODIFIER_SHIFT_L;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			vt->modifiers |= VT240_MODIFIER_SHIFT_R;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			vt->modifiers |= VT240_MODIFIER_CTRL_L;
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			vt->modifiers |= VT240_MODIFIER_CTRL_R;
			break;
		case GLFW_KEY_LEFT_ALT:
			vt->modifiers |= VT240_MODIFIER_ALT_L;
			break;
		case GLFW_KEY_RIGHT_ALT:
			vt->modifiers |= VT240_MODIFIER_ALT_R;
			break;
		default:
			u16 (*translate_key)(VT240*, int);
			switch(vt->config.keyboard) {
				default:
				case VT240_KEYBOARD_NORTH_AMERICAN:
					translate_key = VT240KeyboardTranslateKey1;
					break;
				case VT240_KEYBOARD_BRITISH:
					translate_key = VT240KeyboardTranslateKey2;
					break;
				case VT240_KEYBOARD_FLEMISH:
					translate_key = VT240KeyboardTranslateKey3;
					break;
				case VT240_KEYBOARD_CANADIAN_FRENCH:
					translate_key = VT240KeyboardTranslateKey4;
					break;
				case VT240_KEYBOARD_DANISH:
					translate_key = VT240KeyboardTranslateKey5;
					break;
				case VT240_KEYBOARD_FINNISH:
					translate_key = VT240KeyboardTranslateKey6;
					break;
				case VT240_KEYBOARD_GERMAN:
					translate_key = VT240KeyboardTranslateKey7;
					break;
			}

			u16 value = translate_key(vt, key);
			if(ctrl) {
				if((value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z')) {
					value &= ~0x20; // upper case

					u16 code = value - 'A' + 1;

					vt->repeat_time = 0;
					vt->repeat_state = 0;

					vt->repeat_scancode = key;
					vt->repeat_char = code;

					VT240ProcessKey(vt, code);
				} else {
					u8 code = 0xFF;

					switch(value) {
						case '2':
						case ' ':
							code = NUL;
							break;
						case '3':
						case '[':
							code = ESC;
							break;
						case '4':
						case '\\':
							code = FS;
							break;
						case '5':
						case ']':
							code = GS;
							break;
						case '6':
						case '~':
							code = RS;
							break;
						case '7':
						case '?':
							code = US;
							break;
						case '8':
							code = DEL;
							break;
					}

					if(code != 0xFF) {
						vt->repeat_time = 0;
						vt->repeat_state = 0;

						vt->repeat_scancode = key;
						vt->repeat_char = code;

						VT240ProcessKey(vt, code);
					} else {
						/* no code defined, use default ones */
						u16 code = VT240TranslateKey(vt, key);
						if(code == VT240_KEY_SET_UP) {
							VT240ProcessKey(vt, code);
						} else if(code != 0xFFFF) {
							vt->repeat_time = 0;
							vt->repeat_state = 0;

							vt->repeat_scancode = key;
							vt->repeat_char = 0xFFFF;

							VT240ProcessKey(vt, code);
						} else {
							vt->last_scancode = key;
						}
					}
				}
			} else {
				if(value == VT240_KEY_SET_UP) {
					VT240ProcessKey(vt, value);
				} else if(value != 0xFFFF) {
					vt->repeat_time = 0;
					vt->repeat_state = 0;

					vt->repeat_scancode = key;
					vt->repeat_char = value;

					VT240ProcessKey(vt, value);
				} else {
					vt->last_scancode = key;
				}
			}
			break;
	}
}

void VT240KeyboardKeyUpLocal(VT240* vt, int key)
{
	switch(key) {
		case GLFW_KEY_LEFT_SHIFT:
			vt->modifiers &= ~VT240_MODIFIER_SHIFT_L;
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			vt->modifiers &= ~VT240_MODIFIER_SHIFT_R;
			break;
		case GLFW_KEY_LEFT_CONTROL:
			vt->modifiers &= ~VT240_MODIFIER_CTRL_L;
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			vt->modifiers &= ~VT240_MODIFIER_CTRL_R;
			break;
		case GLFW_KEY_LEFT_ALT:
			vt->modifiers &= ~VT240_MODIFIER_ALT_L;
			break;
		case GLFW_KEY_RIGHT_ALT:
			vt->modifiers &= ~VT240_MODIFIER_ALT_R;
			break;
		default:
			if(vt->repeat_char < 256 && key == vt->repeat_scancode) {
				vt->repeat_char = 0xFFFF;
				vt->repeat_scancode = 0;
			} else if(vt->repeat_scancode == key) {
				vt->repeat_scancode = 0;
			}
			break;
	}
}

void VT240KeyboardKeyDown(VT240* vt, int key)
{
	if(vt->config.keyboard == VT240_KEYBOARD_UNKNOWN) {
		VT240KeyboardKeyDownHost(vt, key);
	} else {
		VT240KeyboardKeyDownLocal(vt, key);
	}
}

void VT240KeyboardKeyUp(VT240* vt, int key)
{
	if(vt->config.keyboard == VT240_KEYBOARD_UNKNOWN) {
		VT240KeyboardKeyUpHost(vt, key);
	} else {
		VT240KeyboardKeyUpLocal(vt, key);
	}
}
