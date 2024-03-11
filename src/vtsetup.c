#include <string.h>

#include "types.h"
#include "vt.h"

#define	SETUP_SCREEN_DIRECTORY	0
#define	SETUP_SCREEN_DISPLAY	1
#define	SETUP_SCREEN_GENERAL	2
#define	SETUP_SCREEN_COMM	3
#define	SETUP_SCREEN_PRINTER	4
#define	SETUP_SCREEN_KEYBOARD	5
#define	SETUP_SCREEN_TAB	6
#define	SETUP_SCREEN_GRAPHICS	7
#define	SETUP_SCREEN_4014	8
#define	SETUP_SCREEN_TELEPHONE	9
#define	SETUP_SCREEN_COUNT	10

static const char* vt240_setup_screen_names[SETUP_SCREEN_COUNT] = {
	"Set-Up Directory",
	"Display Set-Up",
	"General Set-Up",
	"Communications Set-Up",
	"Printer Set-Up",
	"Keyboard Set-Up",
	"Tab Set-Up",
	"Graphics Set-Up",
	"4010/4014 Set-Up",
	"Telephone Set-Up"
};

static const char* vt240_keyboard_languages[16] = {
	"Unknown Keyboard",
	"North American Keyboard",
	"British Keyboard",
	"Flemish Keyboard",
	"Canadian(French) Keyboard",
	"Danish Keyboard",
	"Finnish Keyboard",
	"German Keyboard",
	"Dutch Keyboard",
	"Italian Keyboard",
	"Swiss(French) Keyboard",
	"Swiss(German) Keyboard",
	"Swedish Keyboard",
	"Norwegian Keyboard",
	"French/Belgian Keyboard",
	"Spanish Keyboard"
};

const char* VT240SetupGetTitle(VT240* vt)
{
	if(vt->setup.screen < 0 || vt->setup.screen > 9) {
		return "???";
	} else {
		return vt240_setup_screen_names[vt->setup.screen];
	}
}

void VT240SetupEraseDisplay(VT240* vt)
{
	memset(vt->setup.text, 0x20, 8 * 132 * sizeof(VT240CELL));
	memset(vt->setup.line_attributes, 0, 8);
}

void VT240SetupEraseLine(VT240* vt)
{
	memset(&vt->setup.text[vt->setup.write_y * vt->columns], 0x20, vt->columns * sizeof(VT240CELL));
}

void VT240SetupWrite(VT240* vt, const unsigned char c, const int sgr)
{
	int idx = vt->setup.write_y * vt->columns + vt->setup.write_x;
	vt->setup.text[idx].text = c;
	vt->setup.text[idx].attr = sgr;

	vt->setup.write_x++;
	if(vt->setup.write_x >= vt->columns) {
		vt->setup.write_y++;
		if(vt->setup.write_y >= 8) {
			vt->setup.write_y = 7;
		}
	}
}

void VT240SetupWriteRaw(VT240* vt, const u16 c, const int sgr)
{
	int idx = vt->setup.write_y * vt->columns + vt->setup.write_x;
	vt->setup.text[idx].text = c;
	vt->setup.text[idx].attr = sgr;

	vt->setup.write_x++;
	if(vt->setup.write_x >= vt->columns) {
		vt->setup.write_y++;
		if(vt->setup.write_y >= 8) {
			vt->setup.write_y = 7;
		}
	}
}

void VT240SetupSetLineAttribute(VT240* vt, int attr)
{
	vt->setup.line_attributes[vt->setup.write_y] = attr;
}

void VT240SetupGoto(VT240* vt, const int line, const int column)
{
	vt->setup.write_x = column - 1;
	vt->setup.write_y = line - 1;
}

void VT240SetupCursorRight(VT240* vt)
{
	vt->setup.write_x++;
	if(vt->setup.write_x >= vt->columns) {
		vt->setup.write_x = vt->columns - 1;
	}
}

void VT240SetupCursorRightN(VT240* vt, int n)
{
	vt->setup.write_x += n;
	if(vt->setup.write_x >= vt->columns) {
		vt->setup.write_x = vt->columns - 1;
	}
}

void VT240SetupCursorSave(VT240* vt)
{
	vt->setup.write_x_save = vt->setup.write_x;
	vt->setup.write_y_save = vt->setup.write_y;
}

void VT240SetupCursorRestore(VT240* vt)
{
	vt->setup.write_x = vt->setup.write_x_save;
	vt->setup.write_y = vt->setup.write_y_save;
}

void VT240SetupWriteString(VT240* vt, const char* s, const int sgr)
{
	for(; *s; s++) {
		VT240SetupWrite(vt, *s, sgr);
	}
}

void VT240SetupFillRaw(VT240* vt, const u16 c, unsigned int count, const int sgr)
{
	for(; count; count--) {
		VT240SetupWriteRaw(vt, c, sgr);
	}
}

void VT240SetupWriteNumber(VT240* vt, int val, const int width, const int sgr)
{
	char buf[8];
	int i, j;
	for(i = 7; i > 0 && val > 0; i--) {
		buf[i] = val % 10;
		val /= 10;
	}
	i++;
	j = (7 - i);
	for(; i < 8; i++) {
		VT240SetupWrite(vt, buf[i] + '0', sgr);
	}
	for(; j < width; j++) {
		VT240SetupWrite(vt, ' ', sgr);
	}
}

void VT240SetupShowTitle(VT240* vt)
{
	const char* title = VT240SetupGetTitle(vt);

	VT240SetupGoto(vt, 1, 1);
	VT240SetupEraseLine(vt);
	VT240SetupSetLineAttribute(vt, DECDWL);
	VT240SetupWriteString(vt, title, SGR_BLINKING);

	VT240SetupGoto(vt, 1, 31);
	if(vt->mode & DECSCNM) {
		VT240SetupWriteString(vt, "VT240 V2.2", SGR_UNDERSCORE);
	} else {
		VT240SetupWriteString(vt, "VT240 V2.2", SGR_BOLD | SGR_UNDERSCORE);
	}
}

void VT240SetupShowStatus(VT240* vt)
{
	VT240SetupGoto(vt, 7, 1);
	VT240SetupFillRaw(vt, '-', 80, 0);
	VT240SetupGoto(vt, 8, 2);
	VT240SetupEraseLine(vt);
	if(vt->mode & IRM) {
		VT240SetupWriteString(vt, "Insert Mode", 0);
	} else {
		VT240SetupWriteString(vt, "Replace Mode", 0);
	}

	VT240SetupGoto(vt, 8, 18);
	VT240SetupWriteString(vt, "Printer: None", 0);
}

void VT240SetupShowHint(VT240* vt)
{
	VT240SetupGoto(vt, 8, 2);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, "Press ENTER to take this action - Press Cursor Keys to move", 0);
}

void VT240SetupShowDone(VT240* vt)
{
	VT240SetupGoto(vt, 8, 37);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, "Done", 0);
}

static inline int VT240SetupGetSGR(VT240* vt, int x, int y, int cursor_x, int cursor_y)
{
	if(x == cursor_x && y == cursor_y) {
		if(vt->mode & DECSCNM) {
			return SGR_BOLD;
		} else {
			return SGR_REVERSE | SGR_BOLD;
		}
	} else {
		return SGR_REVERSE;
	}
}

#define GET_SGR(y, x)	VT240SetupGetSGR(vt, x, y, vt->setup.cursor_x, vt->setup.cursor_y)

const char* VT240SetupGetKeyboardLanguage(VT240* vt)
{
	return vt240_keyboard_languages[(int) vt->config.keyboard];
}

void VT240SetupShowDirectory(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			if(vt->setup.cursor_x > 7) {
				vt->setup.cursor_x = 7;
			}
			break;
		case 1:
			if(vt->setup.cursor_x > 5) {
				vt->setup.cursor_x = 5;
			}
			break;
		case 2:
			if(vt->setup.cursor_x > 3) {
				vt->setup.cursor_x = 3;
			}
			break;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Display ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " General ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Comm ", GET_SGR(0, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Printer ", GET_SGR(0, 3));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Keyboard ", GET_SGR(0, 4));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Tab ", GET_SGR(0, 5));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Graphics ", GET_SGR(0, 6));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Telephone ", GET_SGR(0, 7));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	if(vt->config.local) {
		VT240SetupWriteString(vt, " Local   ", GET_SGR(1, 0));
	} else {
		VT240SetupWriteString(vt, " On Line ", GET_SGR(1, 0));
	}
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Clear Display ", GET_SGR(1, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Clear Comm ", GET_SGR(1, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Reset Terminal ", GET_SGR(1, 3));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Recall ", GET_SGR(1, 4));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Save ", GET_SGR(1, 5));

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Set-Up=English ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " ", GET_SGR(2, 1));
	VT240SetupCursorSave(vt);
	VT240SetupWriteString(vt, "                          ", GET_SGR(2, 1));
	VT240SetupCursorRestore(vt);
	VT240SetupWriteString(vt, VT240SetupGetKeyboardLanguage(vt), GET_SGR(2, 1));
	VT240SetupCursorRestore(vt);
	VT240SetupCursorRightN(vt, 27);
	VT240SetupWriteString(vt, " Default ", GET_SGR(2, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Exit ", GET_SGR(2, 3));
}

void VT240SetupShowDisplay(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
		case 1:
			if(vt->setup.cursor_x > 3) {
				vt->setup.cursor_x = 3;
			}
			break;
		case 2:
			if(vt->setup.cursor_x > 1) {
				vt->setup.cursor_x = 1;
			}
			break;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	if(vt->mode & DECCOLM) {
		VT240SetupWriteString(vt, " 132 Columns ", GET_SGR(0, 2));
	} else {
		VT240SetupWriteString(vt, " 80 Columns  ", GET_SGR(0, 2));
	}
	VT240SetupCursorRight(vt);
	switch(vt->config.controls) {
		case VT240_CONTROLS_INTERPRET_CONTROLS:
			VT240SetupWriteString(vt, " Interpret Controls ", GET_SGR(0, 3));
			break;
		case VT240_CONTROLS_DISPLAY_CONTROLS:
			VT240SetupWriteString(vt, " Display Controls   ", GET_SGR(0, 3));
			break;
		case VT240_CONTROLS_REGIS:
			VT240SetupWriteString(vt, " Display ReGIS      ", GET_SGR(0, 3));
			break;
	}

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	if(vt->mode & DECAWM) {
		VT240SetupWriteString(vt, " Auto Wrap    ", GET_SGR(1, 0));
	} else {
		VT240SetupWriteString(vt, " No Auto Wrap ", GET_SGR(1, 0));
	}
	VT240SetupCursorRight(vt);
	if(vt->mode & DECSCLM) {
		VT240SetupWriteString(vt, " Smooth Scroll ", GET_SGR(1, 1));
	} else {
		VT240SetupWriteString(vt, " Jump Scroll   ", GET_SGR(1, 1));
	}
	VT240SetupCursorRight(vt);
	if(vt->mode & DECSCNM) {
		VT240SetupWriteString(vt, " Dark Text, Light Screen ", GET_SGR(1, 2));
	} else {
		VT240SetupWriteString(vt, " Light Text, Dark Screen ", GET_SGR(1, 2));
	}
	VT240SetupCursorRight(vt);
	switch(vt->config.display) {
		case VT240_DISPLAY_MONOCHROME:
			VT240SetupWriteString(vt, " Monochrome Display   ", GET_SGR(1, 3));
			break;
		case VT240_DISPLAY_COLOR:
			VT240SetupWriteString(vt, " Color Display        ", GET_SGR(1, 3));
			break;
		case VT240_DISPLAY_BOTH:
			VT240SetupWriteString(vt, " Mono + Color Display ", GET_SGR(1, 3));
			break;
	}

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	if(vt->mode & DECTCEM) {
		VT240SetupWriteString(vt, " Text Cursor    ", GET_SGR(2, 0));
	} else {
		VT240SetupWriteString(vt, " No Text Cursor ", GET_SGR(2, 0));
	}
	VT240SetupCursorRight(vt);
	if(vt->config.cursor_style == VT240_CURSOR_STYLE_BLOCK_CURSOR) {
		VT240SetupWriteString(vt, " Block Cursor Style     ", GET_SGR(2, 1));
	} else {
		VT240SetupWriteString(vt, " Underline Cursor Style ", GET_SGR(2, 1));
	}
}

void VT240SetupShowGeneral(VT240* vt)
{
	if(vt->vt100_mode) {
		if(vt->setup.cursor_y == 0 && vt->setup.cursor_x > 3) {
			vt->setup.cursor_x = 3;
		} else if(vt->setup.cursor_y > 0 && vt->setup.cursor_x > 2) {
			vt->setup.cursor_x = 2;
		}
	} else if(vt->setup.cursor_x > 2) {
		vt->setup.cursor_x = 2;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	if(!(vt->mode & DECANM)) {
		VT240SetupWriteString(vt, " VT52 Mode                  ", GET_SGR(0, 2));
	} else if(vt->vt100_mode) {
		VT240SetupWriteString(vt, " VT100 Mode                 ", GET_SGR(0, 2));
	} else if(vt->ct_7bit) {
		VT240SetupWriteString(vt, " VT200 Mode, 7 Bit Controls ", GET_SGR(0, 2));
	} else {
		VT240SetupWriteString(vt, " VT200 Mode, 8 Bit Controls ", GET_SGR(0, 2));
	}

	if(vt->vt100_mode) {
		VT240SetupCursorRight(vt);
		switch(vt->config.vt100_terminal_id) {
			case VT240_VT100_TERMINAL_ID_VT240:
				VT240SetupWriteString(vt, " VT240 ID ", GET_SGR(0, 3));
				break;
			case VT240_VT100_TERMINAL_ID_VT100:
				VT240SetupWriteString(vt, " VT100 ID ", GET_SGR(0, 3));
				break;
			case VT240_VT100_TERMINAL_ID_VT101:
				VT240SetupWriteString(vt, " VT101 ID ", GET_SGR(0, 3));
				break;
			case VT240_VT100_TERMINAL_ID_VT102:
				VT240SetupWriteString(vt, " VT102 ID ", GET_SGR(0, 3));
				break;
			case VT240_VT100_TERMINAL_ID_VT125:
				VT240SetupWriteString(vt, " VT125 ID ", GET_SGR(0, 3));
				break;
		}
	}

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	if(vt->udk_locked) {
		VT240SetupWriteString(vt, " User Defined Keys Locked   ", GET_SGR(1, 0));
	} else {
		VT240SetupWriteString(vt, " User Defined Keys Unlocked ", GET_SGR(1, 0));
	}
	VT240SetupCursorRight(vt);
	if(vt->config.user_features == VT240_USER_FEATURES_UNLOCKED) {
		VT240SetupWriteString(vt, " User Features Unlocked ", GET_SGR(1, 1));
	} else {
		VT240SetupWriteString(vt, " User Features Locked   ", GET_SGR(1, 1));
	}
	VT240SetupCursorRight(vt);
	if(vt->config.character_set_mode == VT240_CHARACTER_SET_MODE_MULTINATIONAL) {
		VT240SetupWriteString(vt, " Multinational ", GET_SGR(1, 2));
	} else {
		VT240SetupWriteString(vt, " National      ", GET_SGR(1, 2));
	}

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	if(vt->mode & KAM) {
		VT240SetupWriteString(vt, " Application Keypad ", GET_SGR(2, 0));
	} else {
		VT240SetupWriteString(vt, " Numeric Keypad     ", GET_SGR(2, 0));
	}
	VT240SetupCursorRight(vt);
	if(vt->mode & DECCKM) {
		VT240SetupWriteString(vt, " Application Cursor Keys ", GET_SGR(2, 1));
	} else {
		VT240SetupWriteString(vt, " Normal Cursor Keys      ", GET_SGR(2, 1));
	}
	VT240SetupCursorRight(vt);
	if(vt->mode & LNM) {
		VT240SetupWriteString(vt, " New Line    ", GET_SGR(2, 2));
	} else {
		VT240SetupWriteString(vt, " No New Line ", GET_SGR(2, 2));
	}
}

void VT240SetupShowComm(VT240* vt)
{
	if(vt->setup.cursor_x > 3) {
		vt->setup.cursor_x = 3;
	}
	if(vt->setup.cursor_y == 2 && vt->setup.cursor_x > 2) {
		vt->setup.cursor_x = 2;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Transmit=", GET_SGR(0, 2));
	VT240SetupWriteNumber(vt, 9600, 5, GET_SGR(0, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Receive=Transmit ", GET_SGR(0, 3));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	if(vt->use_xoff) {
		VT240SetupWriteString(vt, " XOFF at ", GET_SGR(1, 0));
		VT240SetupWriteNumber(vt, vt->xoff_point, 4, GET_SGR(1, 0));
	} else {
		VT240SetupWriteString(vt, " No XOFF      ", GET_SGR(1, 0));
	}
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " 8 Bits, No Parity             ", GET_SGR(1, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " 1 Stop Bit  ", GET_SGR(1, 2));
	VT240SetupCursorRight(vt);
	if(vt->mode & SRM) {
		VT240SetupWriteString(vt, " No Local Echo ", GET_SGR(1, 3));
	} else {
		VT240SetupWriteString(vt, " Local Echo    ", GET_SGR(1, 3));
	}

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " EIA Port, Data Leads Only ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Disconnect, 2 s Delay   ", GET_SGR(2, 1));
	VT240SetupCursorRight(vt);
	if(vt->config.transmit == VT240_TRANSMIT_LIMITED) {
		VT240SetupWriteString(vt, " Limited Transmit   ", GET_SGR(2, 2));
	} else {
		VT240SetupWriteString(vt, " Unlimited Transmit ", GET_SGR(2, 2));
	}
}

void VT240SetupShowPrinter(VT240* vt)
{
	if(vt->setup.cursor_x > 3) {
		vt->setup.cursor_x = 3;
	}
	if(vt->setup.cursor_y == 2 && vt->setup.cursor_x > 2) {
		vt->setup.cursor_x = 2;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Speed=4800  ", GET_SGR(0, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " No Printer to Host ", GET_SGR(0, 3));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Normal Print Mode ", GET_SGR(1, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " XOFF    ", GET_SGR(1, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " 7 Bits, Space Parity ", GET_SGR(1, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " 1 Stop Bit  ", GET_SGR(1, 3));

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Print Full Page     ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Print National Only       ", GET_SGR(2, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " No Terminator   ", GET_SGR(2, 2));
}

void VT240SetupShowKeyboard(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			if(vt->setup.cursor_x > 3) {
				vt->setup.cursor_x = 3;
			}
			break;
		case 1:
			if(vt->setup.cursor_x > 4) {
				vt->setup.cursor_x = 4;
			}
			break;
		case 2:
			if(vt->setup.cursor_x > 2) {
				vt->setup.cursor_x = 2;
			}
			break;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	if(vt->config.keys == VT240_KEYS_TYPEWRITER) {
		VT240SetupWriteString(vt, " Typewriter Keys      ", GET_SGR(0, 2));
	} else {
		VT240SetupWriteString(vt, " Data Processing Keys ", GET_SGR(0, 2));
	}
	VT240SetupCursorRight(vt);
	if(vt->config.lock == VT240_LOCK_CAPS_LOCK) {
		VT240SetupWriteString(vt, " Caps Lock  ", GET_SGR(0, 3));
	} else {
		VT240SetupWriteString(vt, " Shift Lock ", GET_SGR(0, 3));
	}

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	if(vt->mode & DECARM) {
		VT240SetupWriteString(vt, " Auto Repeat    ", GET_SGR(1, 0));
	} else {
		VT240SetupWriteString(vt, " No Auto Repeat ", GET_SGR(1, 0));
	}
	VT240SetupCursorRight(vt);
	if(vt->config.keyclick == VT240_KEYCLICK) {
		VT240SetupWriteString(vt, " Keyclick    ", GET_SGR(1, 1));
	} else {
		VT240SetupWriteString(vt, " No Keyclick ", GET_SGR(1, 1));
	}
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " No Margin Bell ", GET_SGR(1, 2));
	VT240SetupCursorRight(vt);
	if(vt->config.bell == VT240_BELL) {
		VT240SetupWriteString(vt, " Warning Bell    ", GET_SGR(1, 3));
	} else {
		VT240SetupWriteString(vt, " No Warning Bell ", GET_SGR(1, 3));
	}
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Break    ", GET_SGR(1, 4));

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " No Auto Answerback ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Answerback=                               ", GET_SGR(2, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Not Concealed ", GET_SGR(2, 2));
}

void VT240SetupShowTab(VT240* vt)
{
	int i;

	if(vt->setup.cursor_y == 0) {
		if(vt->setup.cursor_x > 3) {
			vt->setup.cursor_x = 3;
		}
	} else {
		vt->setup.cursor_y = 1;
		if(vt->setup.cursor_x >= vt->columns) {
			vt->setup.cursor_x = vt->columns - 1;
		}
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Clear All Tabs ", GET_SGR(0, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Set 8 Column Tabs ", GET_SGR(0, 3));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	for(i = 0; i < vt->columns; i++) {
		VT240SetupGoto(vt, 4, i + 1);
		if(i > 0 && vt->tabstops[i - 1]) {
			VT240SetupWriteString(vt, "T", GET_SGR(1, i));
		} else {
			VT240SetupWriteString(vt, " ", GET_SGR(1, i));
		}
		VT240SetupGoto(vt, 5, i + 1);
		VT240SetupWrite(vt, '0' + (i + 1) % 10, ((i / 10) % 2) ? SGR_REVERSE : 0);
	}

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
}

void VT240SetupShowGraphics(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			if(vt->setup.cursor_x > 2) {
				vt->setup.cursor_x = 2;
			}
			break;
		case 1:
			if(vt->setup.cursor_x > 1) {
				vt->setup.cursor_x = 1;
			}
			break;
		case 2:
			if(vt->setup.cursor_x > 3) {
				vt->setup.cursor_x = 3;
			}
			break;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To 4010/4014 Set-Up ", GET_SGR(0, 2));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Graphics Cursor    ", GET_SGR(1, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Macrograph Report    ", GET_SGR(1, 1));

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Compressed Print ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Mono Print  ", GET_SGR(2, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " HLS Print ", GET_SGR(2, 2));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " No Print Background ", GET_SGR(2, 3));
}

void VT240SetupShow4014(VT240* vt)
{
	if(vt->setup.cursor_y == 0) {
		if(vt->setup.cursor_x > 2) {
			vt->setup.cursor_x = 2;
		}
	} else if(vt->setup.cursor_x > 1) {
		vt->setup.cursor_x = 1;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Aligned characters  ", GET_SGR(0, 2));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " CR Effect = CR    ", GET_SGR(1, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " DEL implies Lo Y ", GET_SGR(1, 1));

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " LF Effect = LF    ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " GIN Terminator = None   ", GET_SGR(2, 1));
}

void VT240SetupShowTelephone(VT240* vt)
{
	if(vt->setup.cursor_y == 0) {
		if(vt->setup.cursor_x > 2) {
			vt->setup.cursor_x = 2;
		}
	} else if(vt->setup.cursor_x > 1) {
		vt->setup.cursor_x = 1;
	}

	/* line 1 */
	VT240SetupGoto(vt, 2, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " To Next Set-Up ", GET_SGR(0, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " To Directory ", GET_SGR(0, 1));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Manual Answer ", GET_SGR(0, 2));

	/* line 2 */
	VT240SetupGoto(vt, 4, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Telephone number A=                 ", GET_SGR(1, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Not Concealed ", GET_SGR(1, 1));

	/* line 3 */
	VT240SetupGoto(vt, 6, 1);
	VT240SetupEraseLine(vt);
	VT240SetupWriteString(vt, " Telephone number B=                 ", GET_SGR(2, 0));
	VT240SetupCursorRight(vt);
	VT240SetupWriteString(vt, " Not Concealed ", GET_SGR(2, 1));
}

void VT240SetupShowScreen(VT240* vt)
{
	if(vt->setup.cursor_y < 0) {
		vt->setup.cursor_y = 0;
	}
	if(vt->setup.cursor_y > 2) {
		vt->setup.cursor_y = 2;
	}

	VT240SetupShowTitle(vt);

	/* clear extra stuff from tab setup screen */
	VT240SetupGoto(vt, 5, 1);
	VT240SetupEraseLine(vt);

	switch(vt->setup.screen) {
		case SETUP_SCREEN_DIRECTORY:
			VT240SetupShowDirectory(vt);
			break;
		case SETUP_SCREEN_DISPLAY:
			VT240SetupShowDisplay(vt);
			break;
		case SETUP_SCREEN_GENERAL:
			VT240SetupShowGeneral(vt);
			break;
		case SETUP_SCREEN_COMM:
			VT240SetupShowComm(vt);
			break;
		case SETUP_SCREEN_PRINTER:
			VT240SetupShowPrinter(vt);
			break;
		case SETUP_SCREEN_KEYBOARD:
			VT240SetupShowKeyboard(vt);
			break;
		case SETUP_SCREEN_TAB:
			VT240SetupShowTab(vt);
			break;
		case SETUP_SCREEN_GRAPHICS:
			VT240SetupShowGraphics(vt);
			break;
		case SETUP_SCREEN_4014:
			VT240SetupShow4014(vt);
			break;
		case SETUP_SCREEN_TELEPHONE:
			VT240SetupShowTelephone(vt);
			break;
	}
}

void VT240SetupShow(VT240* vt)
{
	VT240SetupEraseDisplay(vt);
	VT240SetupShowScreen(vt);
	VT240SetupShowStatus(vt);
}

void VT240SetupSetScreen(VT240* vt, int screen)
{
	vt->setup.screen = screen;
	vt->setup.cursor_x = 0;
	vt->setup.cursor_y = 0;
	VT240SetupShowScreen(vt);
}

void VT240SetupNextScreen(VT240* vt)
{
	vt->setup.screen++;
	vt->setup.screen %= SETUP_SCREEN_COUNT;
	if(vt->setup.screen == 0) {
		vt->setup.screen++;
	}
	vt->setup.cursor_x = 0;
	vt->setup.cursor_y = 0;
	VT240SetupShowScreen(vt);
}

void VT240EnterSetup(VT240* vt)
{
	vt->in_setup = 1;
	vt->setup.cursor_x = 0;
	vt->setup.cursor_y = 0;
	vt->setup.screen = SETUP_SCREEN_DIRECTORY;
	vt->setup.state = 0;
	VT240Send(vt, DC3);

	VT240SetupShow(vt);
}

void VT240LeaveSetup(VT240* vt)
{
	vt->in_setup = 0;
	VT240Send(vt, DC1);
}

void VT240SetupDirectoryEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0: /* Display */
					VT240SetupSetScreen(vt, SETUP_SCREEN_DISPLAY);
					break;
				case 1: /* General */
					VT240SetupSetScreen(vt, SETUP_SCREEN_GENERAL);
					break;
				case 2: /* Comm */
					VT240SetupSetScreen(vt, SETUP_SCREEN_COMM);
					break;
				case 3: /* Printer */
					VT240SetupSetScreen(vt, SETUP_SCREEN_PRINTER);
					break;
				case 4: /* Keyboard */
					VT240SetupSetScreen(vt, SETUP_SCREEN_KEYBOARD);
					break;
				case 5: /* Tab */
					VT240SetupSetScreen(vt, SETUP_SCREEN_TAB);
					break;
				case 6: /* Graphics */
					VT240SetupSetScreen(vt, SETUP_SCREEN_GRAPHICS);
					break;
				case 7: /* Telephone */
					VT240SetupSetScreen(vt, SETUP_SCREEN_TELEPHONE);
					break;
			}
			break;
		case 1:
			switch(vt->setup.cursor_x) {
				case 0: /* On Line/Local */
					vt->config.local = !vt->config.local;
					VT240SetupShowScreen(vt);
					break;
				case 1: /* Clear Display */
					VT240EraseInDisplay(vt, 2);
					VT240SetCursor(vt, 1, 1);
					VT240SetupShowDone(vt);
					break;
				case 2:
					/* Clear Comm */
					VT240ClearComm(vt);
					VT240SetupShowDone(vt);
					break;
				case 3: /* Reset */
					VT240SoftReset(vt);
					VT240SetupShowDone(vt);
					break;
				case 4: /* Recall */
					VT240HardReset(vt);
					VT240SetupShow(vt);
					VT240SetupShowDone(vt);
					break;
			}
			break;
		case 2:
			switch(vt->setup.cursor_x) {
				case 1: /* Keyboard language */
					vt->config.keyboard = (vt->config.keyboard + 1) % VT240_KEYBOARD_COUNT;
					VT240SetupShow(vt);
					break;
				case 3: /* Exit */
					VT240LeaveSetup(vt);
					break;
			}
	}
}

void VT240SetupDisplayEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
				case 2:
					if(vt->mode & DECCOLM) {
						VT240ClearColumnMode(vt);
					} else {
						VT240SetColumnMode(vt);
					}
					VT240SetupShow(vt);
					break;
				case 3:
					switch(vt->config.controls) {
						case VT240_CONTROLS_INTERPRET_CONTROLS:
							vt->config.controls = VT240_CONTROLS_DISPLAY_CONTROLS;
							break;
						case VT240_CONTROLS_DISPLAY_CONTROLS:
							vt->config.controls = VT240_CONTROLS_REGIS;
							break;
						case VT240_CONTROLS_REGIS:
							vt->config.controls = VT240_CONTROLS_INTERPRET_CONTROLS;
							break;
					}
					VT240SetupShowScreen(vt);
					break;
			}
			break;
		case 1:
			switch(vt->setup.cursor_x) {
				case 0: /* auto wrap mode */
					vt->mode ^= DECAWM;
					VT240SetupShowScreen(vt);
					break;
				case 1: /* scroll mode */
					vt->mode ^= DECSCLM;
					VT240SetupShowScreen(vt);
					break;
				case 2: /* inverse display */
					vt->mode ^= DECSCNM;
					VT240SetupShowScreen(vt);
					break;
			}
			break;
		case 2:
			switch(vt->setup.cursor_x) {
				case 0: /* text cursor */
					vt->mode ^= DECTCEM;
					VT240SetupShowScreen(vt);
					break;
				case 1: /* cursor style */
					if(vt->config.cursor_style == VT240_CURSOR_STYLE_BLOCK_CURSOR) {
						vt->config.cursor_style = VT240_CURSOR_STYLE_UNDERLINE_CURSOR;
					} else {
						vt->config.cursor_style = VT240_CURSOR_STYLE_BLOCK_CURSOR;
					}
					VT240SetupShowScreen(vt);
					break;
			}
			break;
	}
}

void VT240SetupGeneralEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
				case 2:
					if(!(vt->mode & DECANM)) {
						/* was VT52, go to VT100 */
						vt->mode |= DECANM;
						vt->vt100_mode = 1;
						vt->ct_7bit = 1;
					} else if(vt->vt100_mode) {
						/* was VT100, go to (tek) VT200, 7bit */
						vt->mode |= DECANM;
						vt->ct_7bit = 1;
						vt->vt100_mode = 0;
					} else if(vt->ct_7bit) {
						vt->ct_7bit = 0;
					} else {
						vt->mode &= ~DECANM;
					}
					break;
				case 3:
					switch(vt->config.vt100_terminal_id) {
						case VT240_VT100_TERMINAL_ID_VT240:
							vt->config.vt100_terminal_id = VT240_VT100_TERMINAL_ID_VT100;
							break;
						case VT240_VT100_TERMINAL_ID_VT100:
							vt->config.vt100_terminal_id = VT240_VT100_TERMINAL_ID_VT101;
							break;
						case VT240_VT100_TERMINAL_ID_VT101:
							vt->config.vt100_terminal_id = VT240_VT100_TERMINAL_ID_VT102;
							break;
						case VT240_VT100_TERMINAL_ID_VT102:
							vt->config.vt100_terminal_id = VT240_VT100_TERMINAL_ID_VT125;
							break;
						case VT240_VT100_TERMINAL_ID_VT125:
							vt->config.vt100_terminal_id = VT240_VT100_TERMINAL_ID_VT240;
							break;
					}
					VT240SetupShowScreen(vt);
					break;
			}
			break;
		case 1:
			switch(vt->setup.cursor_x) {
				case 0:
					vt->udk_locked = !vt->udk_locked;
					VT240SetupShowScreen(vt);
					break;
				case 1:
					if(vt->config.user_features == VT240_USER_FEATURES_UNLOCKED) {
						vt->config.user_features = VT240_USER_FEATURES_LOCKED;
					} else {
						vt->config.user_features = VT240_USER_FEATURES_UNLOCKED;
					}
					VT240SetupShowScreen(vt);
					break;
			}
			break;
		case 2:
			switch(vt->setup.cursor_x) {
				case 0: /* Keypad */
					vt->mode ^= KAM;
					VT240SetupShowScreen(vt);
					break;
				case 1: /* Cursor keys */
					vt->mode ^= DECCKM;
					VT240SetupShowScreen(vt);
					break;
				case 2: /* New line */
					vt->mode ^= LNM;
					VT240SetupShowScreen(vt);
					break;
			}
			break;
	}
}

void VT240SetupCommEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
			}
			break;
		case 1:
			switch(vt->setup.cursor_x) {
				case 0:
					switch(vt->xoff_point) {
						case 64:
							vt->xoff_point = 256;
							break;
						case 256:
							vt->xoff_point = 512;
							break;
						case 512:
							vt->xoff_point = 1024;
							break;
						case 1024:
							vt->use_xoff = 0;
							vt->xoff_point = 0;
							break;
						case 0:
							vt->use_xoff = 1;
							vt->xoff_point = 64;
							break;
					}
					VT240SetupShowScreen(vt);
					break;
				case 3:
					vt->mode ^= SRM;
					VT240SetupShowScreen(vt);
					break;
			}
			break;
	}
}

void VT240SetupPrinterEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
			}
			break;
	}
}

void VT240SetupKeyboardEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
			}
			break;
		case 1:
			switch(vt->setup.cursor_x) {
				case 0:
					vt->mode ^= DECARM;
					VT240SetupShowScreen(vt);
					break;
				case 1:
					if(vt->config.keyclick == VT240_KEYCLICK) {
						vt->config.keyclick = VT240_NO_KEYCLICK;
					} else {
						vt->config.keyclick = VT240_KEYCLICK;
					}
					break;
				case 3:
					if(vt->config.bell == VT240_BELL) {
						vt->config.bell = VT240_NO_BELL;
					} else {
						vt->config.bell = VT240_BELL;
					}
					VT240Bell(vt);
					break;
			}
			break;
	}
}

void VT240SetupTabEnter(VT240* vt)
{
	unsigned int i;

	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
				case 2:
					VT240ClearAllTabstops(vt);
					VT240SetupShowScreen(vt);
					break;
				case 3:
					for(i = 0; i < vt->columns; i++) {
						vt->tabstops[i] = i % 8 == 7;
					}
					VT240SetupShowScreen(vt);
					break;
			}
			break;
		case 1:
			if(vt->setup.cursor_x > 0) {
				vt->tabstops[vt->setup.cursor_x - 1] = !vt->tabstops[vt->setup.cursor_x - 1];
			}
			VT240SetupShowScreen(vt);
			break;
	}
}

void VT240SetupGraphicsEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
				case 2:
					VT240SetupSetScreen(vt, SETUP_SCREEN_4014);
					break;
			}
			break;
	}
}

void VT240Setup4014Enter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
			}
			break;
	}
}

void VT240SetupTelephoneEnter(VT240* vt)
{
	switch(vt->setup.cursor_y) {
		case 0:
			switch(vt->setup.cursor_x) {
				case 0:
					VT240SetupNextScreen(vt);
					break;
				case 1:
					VT240SetupSetScreen(vt, SETUP_SCREEN_DIRECTORY);
					break;
			}
			break;
	}
}

void VT240SetupProcessEnter(VT240* vt)
{
	switch(vt->setup.screen) {
		case SETUP_SCREEN_DIRECTORY:
			VT240SetupDirectoryEnter(vt);
			break;
		case SETUP_SCREEN_DISPLAY:
			VT240SetupDisplayEnter(vt);
			break;
		case SETUP_SCREEN_GENERAL:
			VT240SetupGeneralEnter(vt);
			break;
		case SETUP_SCREEN_COMM:
			VT240SetupCommEnter(vt);
			break;
		case SETUP_SCREEN_PRINTER:
			VT240SetupPrinterEnter(vt);
			break;
		case SETUP_SCREEN_KEYBOARD:
			VT240SetupKeyboardEnter(vt);
			break;
		case SETUP_SCREEN_TAB:
			VT240SetupTabEnter(vt);
			break;
		case SETUP_SCREEN_GRAPHICS:
			VT240SetupGraphicsEnter(vt);
			break;
		case SETUP_SCREEN_4014:
			VT240Setup4014Enter(vt);
			break;
		case SETUP_SCREEN_TELEPHONE:
			VT240SetupTelephoneEnter(vt);
			break;
	}
}

#define	STATE_TEXT	0
#define	STATE_ESC	1
#define	STATE_CSI	2
#define	STATE_SS3	3

void VT240SetupProcessKey(VT240* vt, unsigned char c)
{
	switch(vt->setup.state) {
		case STATE_TEXT:
			switch(c) {
				case CR:
					VT240SetupProcessEnter(vt);
					break;
				case LF:
					break;
				case ESC:
					vt->setup.state = STATE_ESC;
					break;
				case CSI:
					vt->setup.state = STATE_CSI;
					break;
				case SS3:
					vt->setup.state = STATE_SS3;
					break;
				default:
					VT240SetupShowHint(vt);
					break;
			}
			break;
		case STATE_ESC:
			switch(c) {
				case ESC:
					vt->setup.state = STATE_ESC;
					break;
				case CSI:
					vt->setup.state = STATE_CSI;
					break;
				case '[':
					vt->setup.state = STATE_CSI;
					break;
				case 'A': /* cursor up (VT52) */
					if(!(vt->mode & DECANM)) {
						vt->setup.state = STATE_TEXT;
						if(vt->setup.cursor_y > 0) {
							vt->setup.cursor_y--;
						}
						VT240SetupShowStatus(vt);
					} else {
						vt->setup.state = STATE_TEXT;
						VT240SetupShowHint(vt);
					}
					break;
				case 'B': /* cursor down (VT52) */
					if(!(vt->mode & DECANM)) {
						vt->setup.state = STATE_TEXT;
						if(vt->setup.cursor_y < 2) {
							vt->setup.cursor_y++;
						}
						VT240SetupShowStatus(vt);
					} else {
						vt->setup.state = STATE_TEXT;
						VT240SetupShowHint(vt);
					}
					break;
				case 'C': /* cursor right (VT52) */
					if(!(vt->mode & DECANM)) {
						vt->setup.state = STATE_TEXT;
						if(vt->setup.cursor_x < vt->columns) {
							vt->setup.cursor_x++;
						}
						VT240SetupShowStatus(vt);
					} else {
						vt->setup.state = STATE_TEXT;
						VT240SetupShowHint(vt);
					}
					break;
				case 'D': /* cursor left (VT52) */
					if(!(vt->mode & DECANM)) {
						vt->setup.state = STATE_TEXT;
						if(vt->setup.cursor_x > 0) {
							vt->setup.cursor_x--;
						}
						VT240SetupShowStatus(vt);
					} else {
						vt->setup.state = STATE_TEXT;
						VT240SetupShowHint(vt);
					}
					break;
				default:
					vt->setup.state = STATE_TEXT;
					VT240SetupShowHint(vt);
					break;
			}
			break;
		case STATE_CSI:
			switch(c) {
				case ESC:
					vt->setup.state = STATE_ESC;
					break;
				case CSI:
					vt->setup.state = STATE_CSI;
					break;
				case 'A': /* cursor up */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_y > 0) {
						vt->setup.cursor_y--;
					}
					VT240SetupShowStatus(vt);
					break;
				case 'B': /* cursor down */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_y < 2) {
						vt->setup.cursor_y++;
					}
					VT240SetupShowStatus(vt);
					break;
				case 'C': /* cursor right */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_x < vt->columns) {
						vt->setup.cursor_x++;
					}
					VT240SetupShowStatus(vt);
					break;
				case 'D': /* cursor left */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_x > 0) {
						vt->setup.cursor_x--;
					}
					VT240SetupShowStatus(vt);
					break;
				default:
					vt->setup.state = STATE_TEXT;
					VT240SetupShowHint(vt);
					break;
			}
			break;
		case STATE_SS3:
			switch(c) {
				case ESC:
					vt->setup.state = STATE_ESC;
					break;
				case CSI:
					vt->setup.state = STATE_CSI;
					break;
				case 'A': /* cursor up */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_y > 0) {
						vt->setup.cursor_y--;
					}
					VT240SetupShowStatus(vt);
					break;
				case 'B': /* cursor down */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_y < 2) {
						vt->setup.cursor_y++;
					}
					VT240SetupShowStatus(vt);
					break;
				case 'C': /* cursor right */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_x < vt->columns) {
						vt->setup.cursor_x++;
					}
					VT240SetupShowStatus(vt);
					break;
				case 'D': /* cursor left */
					vt->setup.state = STATE_TEXT;
					if(vt->setup.cursor_x > 0) {
						vt->setup.cursor_x--;
					}
					VT240SetupShowStatus(vt);
					break;
				default:
					vt->setup.state = STATE_TEXT;
					VT240SetupShowHint(vt);
					break;
			}
			break;
	}

	VT240SetupShowScreen(vt);
}
