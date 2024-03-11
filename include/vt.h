#ifndef __VT_H__
#define __VT_H__

#include <GL/gl.h>

#define	VT240_TEXT_GREEN
#define	VT240_ALTERNATE_INTENSITY

#define	VT240_KEY_HOLD_SCREEN			301
#define	VT240_KEY_PRINT_SCREEN			302
#define	VT240_KEY_SET_UP			303
#define	VT240_KEY_DATA_TALK			304
#define	VT240_KEY_BREAK				305
#define	VT240_KEY_F6				306
#define	VT240_KEY_F7				307
#define	VT240_KEY_F8				308
#define	VT240_KEY_F9				309
#define	VT240_KEY_F10				310
#define	VT240_KEY_F11				311
#define	VT240_KEY_F12				312
#define	VT240_KEY_F13				313
#define	VT240_KEY_F14				314
#define	VT240_KEY_F15				315
#define	VT240_KEY_F16				316
#define	VT240_KEY_F17				317
#define	VT240_KEY_F18				318
#define	VT240_KEY_F19				319
#define	VT240_KEY_F20				320
#define	VT240_KEY_F6_UDK			321
#define	VT240_KEY_F7_UDK			322
#define	VT240_KEY_F8_UDK			323
#define	VT240_KEY_F9_UDK			324
#define	VT240_KEY_F10_UDK			325
#define	VT240_KEY_F11_UDK			326
#define	VT240_KEY_F12_UDK			327
#define	VT240_KEY_F13_UDK			328
#define	VT240_KEY_F14_UDK			329
#define	VT240_KEY_F15_UDK			330
#define	VT240_KEY_F16_UDK			331
#define	VT240_KEY_F17_UDK			332
#define	VT240_KEY_F18_UDK			333
#define	VT240_KEY_F19_UDK			334
#define	VT240_KEY_F20_UDK			335

#define	VT240_KEY_KP_0				336
#define	VT240_KEY_KP_1				337
#define	VT240_KEY_KP_2				338
#define	VT240_KEY_KP_3				339
#define	VT240_KEY_KP_4				340
#define	VT240_KEY_KP_5				341
#define	VT240_KEY_KP_6				342
#define	VT240_KEY_KP_7				343
#define	VT240_KEY_KP_8				344
#define	VT240_KEY_KP_9				345
#define	VT240_KEY_KP_MINUS			346
#define	VT240_KEY_KP_COMMA			347
#define	VT240_KEY_KP_PERIOD			348
#define	VT240_KEY_KP_ENTER			349
#define	VT240_KEY_KP_PF1			350
#define	VT240_KEY_KP_PF2			351
#define	VT240_KEY_KP_PF3			352
#define	VT240_KEY_KP_PF4			353

#define	VT240_KEY_FIND				354
#define	VT240_KEY_INSERT			355
#define	VT240_KEY_REMOVE			356
#define	VT240_KEY_SELECT			357
#define	VT240_KEY_PREV_SCREEN			358
#define	VT240_KEY_NEXT_SCREEN			359
#define	VT240_KEY_UP				360
#define	VT240_KEY_DOWN				361
#define	VT240_KEY_LEFT				362
#define	VT240_KEY_RIGHT				363

#define	VT240_KEY_CTRL				364
#define	VT240_KEY_SHIFT				365

#define	VT240_MODIFIER_SHIFT_L			_BV(0)
#define	VT240_MODIFIER_SHIFT_R			_BV(1)
#define	VT240_MODIFIER_CTRL_L			_BV(2)
#define	VT240_MODIFIER_CTRL_R			_BV(3)
#define	VT240_MODIFIER_ALT_L			_BV(4)
#define	VT240_MODIFIER_ALT_R			_BV(5)

#define	MAX_PARAMETERS				16

#define	VT240_LANGUAGE_ENGLISH			0
#define	VT240_LANGUAGE_FRANCAIS			1
#define	VT240_LANGUAGE_DEUTSCH			2

#define	VT240_KEYBOARD_UNKNOWN			0
#define	VT240_KEYBOARD_NORTH_AMERICAN		1
#define	VT240_KEYBOARD_BRITISH			2
#define	VT240_KEYBOARD_FLEMISH			3
#define	VT240_KEYBOARD_CANADIAN_FRENCH		4
#define	VT240_KEYBOARD_DANISH			5
#define	VT240_KEYBOARD_FINNISH			6
#define	VT240_KEYBOARD_GERMAN			7
#define	VT240_KEYBOARD_DUTCH			8
#define	VT240_KEYBOARD_ITALIAN			9
#define	VT240_KEYBOARD_SWISS_FRENCH		10
#define	VT240_KEYBOARD_SWISS_GERMAN		11
#define	VT240_KEYBOARD_SWEDISH			12
#define	VT240_KEYBOARD_NORWEGIAN		13
#define	VT240_KEYBOARD_FRENCH			14
#define	VT240_KEYBOARD_SPANISH			15
#define	VT240_KEYBOARD_COUNT			16

#define	VT240_CONTROLS_INTERPRET_CONTROLS	0
#define	VT240_CONTROLS_DISPLAY_CONTROLS		1
#define	VT240_CONTROLS_REGIS			2

#define	VT240_SCROLL_SMOOTH_SCROLL		0
#define	VT240_SCROLL_JUMP_SCROLL		1
#define	VT240_SCROLL_NO_SCROLL			2

#define	VT240_TEXT_LIGHT_TEXT			0
#define	VT240_TEXT_DARK_TEXT			1

#define	VT240_DISPLAY_MONOCHROME		0
#define	VT240_DISPLAY_COLOR			1
#define	VT240_DISPLAY_BOTH			2

#define	VT240_TEXT_CURSOR			0
#define	VT240_NO_TEXT_CURSOR			1

#define	VT240_CURSOR_STYLE_BLOCK_CURSOR		0
#define	VT240_CURSOR_STYLE_UNDERLINE_CURSOR	1

#define	VT240_MODE_VT200_MODE_7BIT_CONTROLS	0
#define	VT240_MODE_VT200_MODE_8BIT_CONTROLS	1
#define	VT240_MODE_VT52_MODE			2
#define	VT240_MODE_VT100_MODE			3
#define	VT240_MODE_4010_4014_MODE		4

#define	VT240_VT100_TERMINAL_ID_VT240		0
#define	VT240_VT100_TERMINAL_ID_VT100		1
#define	VT240_VT100_TERMINAL_ID_VT101		2
#define	VT240_VT100_TERMINAL_ID_VT102		3
#define	VT240_VT100_TERMINAL_ID_VT125		4

#define	VT240_USER_DEFINED_KEYS_UNLOCKED	0
#define	VT240_USER_DEFINED_KEYS_LOCKED		1

#define	VT240_USER_FEATURES_UNLOCKED		0
#define	VT240_USER_FEATURES_LOCKED		1

#define	VT240_CHARACTER_SET_MODE_MULTINATIONAL	0
#define	VT240_CHARACTER_SET_MODE_NATIONAL	1

#define	VT240_KEYPAD_NUMERIC			0
#define	VT240_KEYPAD_APPLICATION		1

#define	VT240_CURSOR_KEYS_NORMAL		0
#define	VT240_CURSOR_KEYS_APPLICATION		1

#define	VT240_NO_NEW_LINE			0
#define	VT240_NEW_LINE				1

#define	VT240_XOFF_64				0
#define	VT240_XOFF_256				1
#define	VT240_XOFF_512				2
#define	VT240_XOFF_1024				3
#define	VT240_XOFF_OFF				4

#define	VT240_NO_LOCAL_ECHO			0
#define	VT240_LOCAL_ECHO			1

#define	VT240_TRANSMIT_LIMITED			0
#define	VT240_TRANSMIT_UNLIMITED		1

#define	VT240_KEYS_TYPEWRITER			0
#define	VT240_KEYS_DATA_PROCESSING		1

#define	VT240_LOCK_CAPS_LOCK			0
#define	VT240_LOCK_SHIFT_LOCK			1

#define	VT240_AUTO_REPEAT			0
#define	VT240_NO_AUTO_REPEAT			1

#define	VT240_KEYCLICK				0
#define	VT240_NO_KEYCLICK			1

#define	VT240_BELL				0
#define	VT240_NO_BELL				1

typedef struct {
	char		local;
	char		language;
	char		keyboard;
	char		columns;
	char		controls;
	char		auto_wrap;
	char		scroll;
	char		text;
	char		display;
	char		text_cursor;
	char		cursor_style;
	char		mode;
	char		vt100_terminal_id;
	char		user_features;
	char		character_set_mode;
	char		keypad;
	char		cursor_keys;
	char		new_line;
	char		xoff;
	char		local_echo;
	char		transmit;
	char		keys;
	char		lock;
	char		auto_repeat;
	char		keyclick;
	char		bell;
} VT240NVR;

typedef struct {
	u16		text;
	u16		attr;
} VT240CELL;

typedef struct {
	int		cursor_x;
	int		cursor_y;
	int		screen;
	int		status;
	int		state;
	int		write_x;
	int		write_y;
	int		write_x_save;
	int		write_y_save;
	VT240CELL*	text;
	char*		line_attributes;
} VT240Setup;

typedef struct {
	int		lines;
	int		columns;
	VT240CELL*	text;
	u32*		framebuffer;
	int		fb_dirty;
	char*		line_attributes;
	unsigned char*	buf;
	int		buf_r;
	int		buf_w;
	int		buf_used;
	int		buf_lost;
	int		state;
	u16		parameters[MAX_PARAMETERS];
	int		parameter_id;
	int		cursor_x;
	int		cursor_y;
	int		g[4];
	int		gl;
	int		gr;
	int		gl_lock;
	int		gr_lock;
	int		g_dst;
	char*		tabstops;
	int		ct_7bit;

	int		vt100_mode;

	int		use_xoff;
	int		xoff;
	int		xoff_point;
	int		xon_point;
	int		sent_xoff;
	u32		mode;

	char		answerback[31];

	int		margin_top;
	int		margin_bottom;

	unsigned long	cursor_time;

	int		sgr;

	/* sixel state */
	int		sixel_bg;
	int		sixel_x;
	int		sixel_y;

	/* saved state */
	int		saved_cursor_x;
	int		saved_cursor_y;
	int		saved_sgr;
	int		saved_awm;
	int		saved_om;
	int		saved_gl;
	int		saved_gr;
	int		saved_gl_lock;
	int		saved_gr_lock;
	int		saved_g[4];

	/* keyboard */
	unsigned long	repeat_time;
	int		repeat_state;

	u32		modifiers;
	int		last_scancode;
	u16		repeat_scancode;
	u16		repeat_char;

	/* user defined keys */
	u8		udk_key;
	u8		udk_hex;

	int		udk_locked;
	u8		udk_ptr[15];
	u16		udk_len[15];
	char		udk_memory[256];
	u16		udk_free;

	/* configuration */
	VT240NVR	config;

	/* setup screens  */
	int		in_setup;
	VT240Setup	setup;

	/* callbacks */
	void		(*bell)(void);
	void		(*keyclick)(void);
	void		(*rx)(unsigned char);
	void		(*resize)(unsigned int width, unsigned int height);
} VT240;

#define	CHARSET_DMCS	0 /* Multilangual */
#define	CHARSET_DEC	1 /* DEC Special Characters */
#define	CHARSET_CNTL	2 /* Display Controls Font */

#define	CHARSET_ASCII			0
#define	CHARSET_DEC_SUPPLEMENTAL	1
#define	CHARSET_DEC_SPECIAL_GRAPHICS	2
#define	CHARSET_NRCS_BRITISH		3
#define	CHARSET_NRCS_DUTCH		4
#define	CHARSET_NRCS_FINNISH		5
#define	CHARSET_NRCS_FRENCH		6
#define	CHARSET_NRCS_FRENCH_CANADIAN	7
#define	CHARSET_NRCS_GERMAN		8
#define	CHARSET_NRCS_ITALIAN		9
#define	CHARSET_NRCS_NORWEGIAN		10
#define	CHARSET_NRCS_SPANISH		11
#define	CHARSET_NRCS_SWEDISH		12
#define	CHARSET_NRCS_SWISS		13
#define	CHARSET_ASCII_DC		14
#define	CHARSET_DEC_SUPPLEMENTAL_DC	15

#define	NUL		0x00
#define	SOH		0x01
#define	STX		0x02
#define	ETX		0x03
#define	EOT		0x04
#define	ENQ		0x05
#define	ACK		0x06
#define	BEL		0x07
#define	BS		0x08
#define	HT		0x09
#define	LF		0x0A
#define	VT		0x0B
#define	FF		0x0C
#define	CR		0x0D
#define	SO		0x0E
#define	SI		0x0F
#define	DLE		0x10
#define	DC1		0x11
#define	DC2		0x12
#define	DC3		0x13
#define	DC4		0x14
#define	NAK		0x15
#define	SYN		0x16
#define	ETB		0x17
#define	CAN		0x18
#define	EM		0x19
#define	SUB		0x1A
#define	ESC		0x1B
#define	FS		0x1C
#define	GS		0x1D
#define	RS		0x1E
#define	US		0x1F
#define	DEL		0x7F
#define	IND		0x84
#define	NEL		0x85
#define	HTS		0x88
#define	RI		0x8D
#define	SS2		0x8E
#define	SS3		0x8F
#define	DCS		0x90
#define	CSI		0x9B
#define	ST		0x9C

#define _BV(x)		(1 << (x))
#define	KAM		_BV(0)
#define	IRM		_BV(1)
#define	SRM		_BV(2)
#define	LNM		_BV(3)
#define	DECCKM		_BV(4)
#define	DECANM		_BV(5)
#define	DECCOLM		_BV(6)
#define	DECSCLM		_BV(7)
#define	DECSCNM		_BV(8)
#define	DECOM		_BV(9)
#define	DECAWM		_BV(10)
#define	DECARM		_BV(11)
#define	DECPFF		_BV(11)
#define	DECPEX		_BV(12)
#define	DECTCEM		_BV(13)
#define	DECKPAM		_BV(14)
#define	DECTEK		_BV(15)
#define	DECNRCM		_BV(16)
#define	DECGEPM		_BV(17)
#define	DECGPCM		_BV(18)
#define	DECGPCS		_BV(19)
#define	DECGPBM		_BV(20)
#define	DECGRPM		_BV(21)

#define	SGR_BOLD	_BV(0)
#define	SGR_UNDERSCORE	_BV(1)
#define	SGR_BLINKING	_BV(2)
#define	SGR_REVERSE	_BV(3)
#define	SCA_ON		_BV(4)

#define	DECSWL		0
#define	DECDHL_TOP	1
#define	DECDHL_BOTTOM	2
#define	DECDWL		3

#define	EL_TO_END	0
#define	EL_FROM_BOL	1
#define	EL_WHOLE_LINE	2

#define	ED_TO_END	0
#define	ED_FROM_BEGIN	1
#define	ED_ALL		2

int* VT240CreateTextureSheet(void);

void VT240Init(VT240* vt);
void VT240Destroy(VT240* vt);
void VT240Process(VT240* vt, unsigned long dt);
void VT240Receive(VT240* vt, unsigned char c);
void VT240ReceiveText(VT240* vt, const char* s);
void VT240SetTextureFilter(VT240* vt, bool linear);
void VT240Draw(VT240* vt);

/* private functions */
void VT240Write(VT240* vt, u16 c);
void VT240WriteChar(VT240* vt, unsigned char c);
void VT240WriteCS(VT240* vt, unsigned char c, int cs);
void VT240CursorHome(VT240* vt);
void VT240CursorLeft(VT240* vt);
void VT240CursorLeftN(VT240* vt, int n);
void VT240CursorRight(VT240* vt);
void VT240CursorRightN(VT240* vt, int n);
void VT240CursorDown(VT240* vt);
void VT240CursorDownN(VT240* vt, int n);
void VT240CursorUp(VT240* vt);
void VT240CursorUpN(VT240* vt, int n);
void VT240SetCursor(VT240* vt, int line, int column);
void VT240SaveCursor(VT240* vt);
void VT240RestoreCursor(VT240* vt);
void VT240Tabstop(VT240* vt);
void VT240SetTabstop(VT240* vt);
void VT240ClearTabstop(VT240* vt);
void VT240ClearAllTabstops(VT240* vt);
void VT240CarriageReturn(VT240* vt);
void VT240Linefeed(VT240* vt);
void VT240ReverseLinefeed(VT240* vt);
void VT240InsertLine(VT240* vt);
void VT240InsertLineN(VT240* vt, int n);
void VT240DeleteLine(VT240* vt);
void VT240DeleteLineN(VT240* vt, int n);
void VT240InsertCharacter(VT240* vt);
void VT240InsertCharacterN(VT240* vt, int n);
void VT240DeleteCharacter(VT240* vt);
void VT240DeleteCharacterN(VT240* vt, int n);
void VT240EraseCharacter(VT240* vt, int n);
void VT240EraseInLine(VT240* vt, int type);
void VT240EraseInDisplay(VT240* vt, int type);
void VT240SelectiveEraseInLine(VT240* vt, int type);
void VT240SetColumnMode(VT240* vt);
void VT240ClearColumnMode(VT240* vt);
void VT240SetVT52Mode(VT240* vt);
void VT240SetANSIMode(VT240* vt);
void VT240ShiftOut(VT240* vt);
void VT240ShiftIn(VT240* vt);
void VT240Xon(VT240* vt);
void VT240Xoff(VT240* vt);
void VT240Substitute(VT240* vt);
void VT240Index(VT240* vt);
void VT240NextLine(VT240* vt);
void VT240ReverseIndex(VT240* vt);
void VT240SetLineMode(VT240* vt, int mode);
void VT240SingleShiftG2(VT240* vt);
void VT240SingleShiftG3(VT240* vt);
void VT240SendAnswerback(VT240* vt);
void VT240SendPrimaryDA(VT240* vt);
void VT240SendSecondaryDA(VT240* vt);
void VT240Send(VT240* vt, unsigned char c);
void VT240SendText(VT240* vt, char* c);
void VT240SendDecimal(VT240* vt, int x);
void VT240Bell(VT240* vt);
void VT240Keyclick(VT240* vt);
void VT240ScrollUp(VT240* vt);
void VT240ScrollDown(VT240* vt);
void VT240SetTopBottomMargins(VT240* vt, int top, int bottom);
void VT240EraseScreen(VT240* vt);
void VT240ClearComm(VT240* vt);
void VT240SoftReset(VT240* vt);
void VT240HardReset(VT240* vt);
void VT240ClearUDK(VT240* vt, unsigned int key);
void VT240ClearAllUDK(VT240* vt);
int  VT240GetCharset(VT240* vt, unsigned char c);
void VT240ProcessChar(VT240* vt, unsigned char c);
void VT240ProcessKey(VT240* vt, u16 key);
void VT240ProcessKeys(VT240* vt, unsigned long dt);

/* keyboard */
void VT240InitKeyboard(VT240* vt);
void VT240KeyboardKeyDown(VT240* vt, int key);
void VT240KeyboardKeyUp(VT240* vt, int key);
void VT240KeyboardChar(VT240* vt, unsigned int code);
void VT240KeyboardProcess(VT240* vt, unsigned long dt);

/* setup functions */
void VT240EnterSetup(VT240* vt);
void VT240LeaveSetup(VT240* vt);
void VT240SetupProcessKey(VT240* vt, unsigned char c);

#endif
