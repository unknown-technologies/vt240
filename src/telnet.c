#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#endif

#include "types.h"
#include "telnet.h"

#define	SE		240
#define	NOP		241
#define	DATA_MARK	242
#define	BRK		243
#define	IP		244
#define	AO		245
#define	AYT		246
#define	EC		247
#define	EL		248
#define	GA		249
#define	SB		250
#define	WILL		251
#define	WONT		252
#define	DO		253
#define	DONT		254
#define	IAC		255

#define	ECHO		1
#define	SUPPRESS_GA	3
#define	TERMINAL_TYPE	24
#define	ENVIRON		36
#define	NEW_ENVIRON	39

#define	IS		0
#define	SEND		1
#define	INFO		2

#define	VAR		0
#define	VALUE		1
#define	ESC		2
#define	USERVAR		3

#define	STATE_TEXT	0
#define	STATE_IAC	1
#define	STATE_WILL	2
#define	STATE_WONT	3
#define	STATE_DO	4
#define	STATE_DONT	5
#define	STATE_SB	6
#define	STATE_SB_IAC	7
#define	STATE_FAIL	10

#define	BUFFER_SIZE	256

static const char* telnet_option_names[50] = {
	/* 0x00 */ "TRANSMIT-BINARY",
	/* 0x01 */ "ECHO",
	/* 0x02 */ "RECONNECT",
	/* 0x03 */ "SUPPRESS-GO-AHEAD",
	/* 0x04 */ "APPROX-MESSAGE-SIZE-NEGOTIATION",
	/* 0x05 */ "STATUS",
	/* 0x06 */ "TIMING-MARK",
	/* 0x07 */ "RCTE",
	/* 0x08 */ "OUTPUT-LINE-WIDTH",
	/* 0x09 */ "OUTPUT-PAGE-SIZE",
	/* 0x0A */ "NAOCRD",
	/* 0x0B */ "NAOHTS",
	/* 0x0C */ "NAOHTD",
	/* 0x0D */ "NAOFFD",
	/* 0x0E */ "NAOVTS",
	/* 0x0F */ "NAOVTD",
	/* 0x10 */ "NAOLFD",
	/* 0x11 */ "EXTEND-ASCII",
	/* 0x12 */ "LOGOUT",
	/* 0x13 */ "BM",
	/* 0x14 */ "DET",
	/* 0x15 */ "SUPDUP",
	/* 0x16 */ "SUPDUP-OUTPUT",
	/* 0x17 */ "SEND-LOCATION",
	/* 0x18 */ "TERMINAL-TYPE",
	/* 0x19 */ "END-OF-RECORD",
	/* 0x1A */ "TUID",
	/* 0x1B */ "OUTMRK",
	/* 0x1C */ "TTYLOC",
	/* 0x1D */ "3270-REGIME",
	/* 0x1E */ "X.3-PAD",
	/* 0x1F */ "NAWS",
	/* 0x20 */ "TERMINAL-SPEED",
	/* 0x21 */ "TOGGLE-FLOW-CONTROL",
	/* 0x22 */ "LINEMODE",
	/* 0x23 */ "X-DISPLAY-LOCATION",
	/* 0x24 */ "ENVIRON",
	/* 0x25 */ "AUTHENTICATION",
	/* 0x26 */ "ENCRYPT",
	/* 0x27 */ "NEW-ENVIRON",
	/* 0x28 */ "TN3270E",
	/* 0x29 */ "XAUTH",
	/* 0x2A */ "CHARSET",
	/* 0x2B */ "RSP",
	/* 0x2C */ "COM-PORT-OPTION",
	/* 0x2D */ "SUPPRESS-LOCAL-ECHO",
	/* 0x2E */ "STARTTLS",
	/* 0x2F */ "KERMIT",
	/* 0x30 */ "SEND-URL",
	/* 0x31 */ "FORWARD_X"
};

#ifdef _WIN32
static WSADATA wsa;
static bool wsa_initialized = false;

#define MSG_NOSIGNAL	0
#define	close		closesocket

const char* hstrerror(int err)
{
	const char* s;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&s, 0, NULL);
	return s;
}

const char* geterror(void)
{
	const char* s;
	int err = WSAGetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&s, 0, NULL);
	return s;
}
#else
const char* geterror(void)
{
	return strerror(errno);
}
#endif

const char* TELNETGetOptionName(unsigned char c)
{
	if(c < 50) {
		return telnet_option_names[c];
	} else {
		return "???";
	}
}

void TELNETInit(TELNET* telnet)
{
	telnet->socket = -1;
	telnet->connected = 0;
	telnet->state = 0;

	telnet->buf = (unsigned char*) malloc(BUFFER_SIZE);
	telnet->read_ptr = 0;
	telnet->write_ptr = 0;

	telnet->sb_buf = (unsigned char*) malloc(BUFFER_SIZE);
	telnet->sb_write_ptr = 0;

#ifdef _WIN32
	if(!wsa_initialized) {
		wsa_initialized = true;
		if(WSAStartup(MAKEWORD(2, 2), &wsa) != NO_ERROR) {
			telnet->state = STATE_FAIL;
		}
	}
#endif
}

void TELNETRxString(TELNET* telnet, const char* s)
{
	for(; *s; s++) {
		telnet->rx(*s);
	}
}

void TELNETRxError(TELNET* telnet, const char* what, const char* msg)
{
	TELNETRxString(telnet, what);
	TELNETRxString(telnet, " failed: ");
	TELNETRxString(telnet, msg);
	TELNETRxString(telnet, "\r\n");

	printf("[TELNET] %s failed: %s\n", what, msg);
}

void TELNETConnected(TELNET* telnet)
{
	telnet->connected = 1;
	TELNETRxString(telnet, "Connected.\r\n");
}

void TELNETConnect(TELNET* telnet, const char* hostname, int port)
{
	struct sockaddr_in sin;
	struct hostent* he;
	memset(&sin, 0, sizeof(sin));

	telnet->socket = -1;

	if(telnet->state == STATE_FAIL) {
		return;
	}

	he = gethostbyname(hostname);
	if(!he) {
		const char* str = hstrerror(h_errno);
		TELNETRxError(telnet, "gethostbyname", str);
		return;
	}

	telnet->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(telnet->socket == -1) {
		const char* str = geterror();
		TELNETRxError(telnet, "socket", str);
		return;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr = *((struct in_addr*) he->h_addr);

#ifdef _WIN32
	u_long mode = 1;
	if(ioctlsocket(telnet->socket, FIONBIO, &mode) != NO_ERROR) {
		const char* str = geterror();
		close(telnet->socket);
		telnet->socket = -1;
		TELNETRxError(telnet, "ioctlsocket", str);
		return;
	}
#else
	int flags = fcntl(telnet->socket, F_GETFL, 0);
	if(flags == -1) {
		const char* str = geterror();
		close(telnet->socket);
		telnet->socket = -1;
		TELNETRxError(telnet, "fcntl", str);
		return;
	}

	if(fcntl(telnet->socket, F_SETFL, flags | O_NONBLOCK) == -1) {
		const char* str = geterror();
		close(telnet->socket);
		telnet->socket = -1;
		TELNETRxError(telnet, "fcntl", str);
		return;
	}
#endif

	if(connect(telnet->socket, (struct sockaddr*) &sin, sizeof(struct sockaddr)) == -1) {
#ifdef _WIN32
		if(WSAGetLastError() != WSAEWOULDBLOCK) {
#else
		if(errno != EINPROGRESS) {
#endif
			const char* str = geterror();
			close(telnet->socket);
			telnet->socket = -1;
			TELNETRxError(telnet, "connect", str);
			return;
		}
	} else {
		TELNETConnected(telnet);
	}
}

void TELNETDisconnect(TELNET* telnet)
{
	if(telnet->socket != -1) {
		close(telnet->socket);
		telnet->socket = -1;
	}
}

#ifdef _WIN32
#define	WINCAST	(char*)
#else
#define	WINCAST
#endif

void TELNETPollConnect(TELNET* telnet)
{
	fd_set wfds;
	struct timeval timeout;

	FD_ZERO(&wfds);
	FD_SET(telnet->socket, &wfds);

	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;

	int sel = select(telnet->socket + 1, NULL, &wfds, NULL, &timeout);
	if(sel == -1) {
		const char* str = geterror();
		close(telnet->socket);
		telnet->socket = -1;
		TELNETRxError(telnet, "select", str);
		return;
	} else if(sel == 0) {
		// timeout
		return;
	} else if(FD_ISSET(telnet->socket, &wfds)) {
		TELNETConnected(telnet);
	}
}

void TELNETSend(TELNET* telnet, unsigned char c)
{
	if(c == IAC) {
		TELNETSendRaw(telnet, IAC);
		TELNETSendRaw(telnet, IAC);
	} else {
		TELNETSendRaw(telnet, c);
	}
}

void TELNETSendRaw(TELNET* telnet, unsigned char c)
{
	if(telnet->socket == -1) {
		return;
	}

	if(!telnet->connected) {
		TELNETPollConnect(telnet);
		if(telnet->socket == -1) {
			return;
		}

		if(!telnet->connected) {
			telnet->count++;
			telnet->buf[telnet->write_ptr++] = c;
			telnet->write_ptr %= BUFFER_SIZE;
			return;
		}
	} else if(telnet->count > 0) {
		while(telnet->count > 0) {
			unsigned char ch = telnet->buf[telnet->read_ptr++];
			telnet->read_ptr %= BUFFER_SIZE;
			telnet->count--;

			if(send(telnet->socket, WINCAST &ch, 1, MSG_NOSIGNAL) == -1) {
				const char* str = geterror();
				close(telnet->socket);
				telnet->socket = -1;
				TELNETRxError(telnet, "send", str);
				return;
			}
		}
	}

	if(send(telnet->socket, WINCAST &c, 1, MSG_NOSIGNAL) == -1) {
		const char* str = geterror();
		close(telnet->socket);
		telnet->socket = -1;
		TELNETRxError(telnet, "send", str);
	}
}

void TELNETSendRawString(TELNET* telnet, const char* s)
{
	for(; *s; s++) {
		TELNETSendRaw(telnet, *s);
	}
}

void TELNETProcessSB(TELNET* telnet)
{
	printf("[TELNET] SB %s [%d]\n", TELNETGetOptionName(telnet->sb_buf[0]), telnet->sb_buf[0]);
	switch(telnet->sb_buf[0]) {
		case TERMINAL_TYPE:
			if(telnet->sb_write_ptr == 2 && telnet->sb_buf[1] == SEND) {
				TELNETSendRaw(telnet, IAC);
				TELNETSendRaw(telnet, SB);
				TELNETSendRaw(telnet, TERMINAL_TYPE);
				TELNETSendRaw(telnet, IS);
				TELNETSendRawString(telnet, "DEC-VT240");
				TELNETSendRaw(telnet, IAC);
				TELNETSendRaw(telnet, SE);
			}
			break;
		case NEW_ENVIRON:
			if(telnet->sb_buf[1] == SEND) {
				int i = 0;
				int var_start = 0;
				int var_type = 0;
				int state = 0;
				for(i = 2; i < telnet->sb_write_ptr; i++) {
					switch(state) {
						case 0: /* type */
							state = 1;
							var_type = telnet->sb_buf[i];
							var_start = i + 1;
							break;
						case 1: /* name */
							switch(telnet->sb_buf[i]) {
								case VAR:
								case USERVAR:
									/* undefined */
									state = 1;
									var_type = telnet->sb_buf[i];
									telnet->sb_buf[i] = 0;
									printf("[TELNET] VAR [%d]: %s\n", var_type, &telnet->sb_buf[var_start]);
									var_start = i + 1;
									break;
								/* default: nothing */
							}
							break;
					}
				}
				if(state == 1) {
					telnet->sb_buf[i] = 0;
					printf("[TELNET] VAR [%d]: %s\n", var_type, &telnet->sb_buf[var_start]);
				}

				TELNETSendRaw(telnet, IAC);
				TELNETSendRaw(telnet, SB);
				TELNETSendRaw(telnet, NEW_ENVIRON);
				TELNETSendRaw(telnet, IS);
#if 0
				TELNETSendRaw(telnet, USERVAR);
				TELNETSendRawString(telnet, "TERM");
				TELNETSendRaw(telnet, VALUE);
				TELNETSendRawString(telnet, "vt220");
				TELNETSendRaw(telnet, USERVAR);
				TELNETSendRawString(telnet, "LANG");
				TELNETSendRaw(telnet, VALUE);
				TELNETSendRawString(telnet, "C");
#endif
				TELNETSendRaw(telnet, IAC);
				TELNETSendRaw(telnet, SE);
			}
			break;
	}

}

void TELNETProcess(TELNET* telnet, unsigned char c)
{
	switch(telnet->state) {
		case STATE_TEXT:
			switch(c) {
				case IAC:
					telnet->state = STATE_IAC;
					break;
				default:
					telnet->rx(c);
					break;
			}
			break;
		case STATE_IAC:
			switch(c) {
				case IAC:
					telnet->state = STATE_TEXT;
					telnet->rx(c);
					break;
				case WILL:
					telnet->state = STATE_WILL;
					break;
				case WONT:
					telnet->state = STATE_WONT;
					break;
				case DO:
					telnet->state = STATE_DO;
					break;
				case DONT:
					telnet->state = STATE_DONT;
					break;
				case SB:
					telnet->state = STATE_SB;
					telnet->sb_write_ptr = 0;
					break;
				default:
					telnet->state = STATE_TEXT;
					break;
			}
			break;
		case STATE_WILL:
			telnet->state = STATE_TEXT;
			printf("[TELNET] WILL %s [%d]\n", TELNETGetOptionName(c), c);
			switch(c) {
				case ECHO:
				case SUPPRESS_GA:
				case TERMINAL_TYPE:
					TELNETSendRaw(telnet, IAC);
					TELNETSendRaw(telnet, DO);
					TELNETSendRaw(telnet, c);
					break;
				default:
					TELNETSendRaw(telnet, IAC);
					TELNETSendRaw(telnet, DONT);
					TELNETSendRaw(telnet, c);
			}
			break;
		case STATE_WONT:
			telnet->state = STATE_TEXT;
			printf("[TELNET] WONT %s [%d]\n", TELNETGetOptionName(c), c);
			TELNETSendRaw(telnet, IAC);
			TELNETSendRaw(telnet, DONT);
			TELNETSendRaw(telnet, c);
			break;
		case STATE_DO:
			telnet->state = STATE_TEXT;
			printf("[TELNET] DO %s [%d]\n", TELNETGetOptionName(c), c);
			switch(c) {
				case ECHO:
				case SUPPRESS_GA:
				case TERMINAL_TYPE:
				case NEW_ENVIRON:
					TELNETSendRaw(telnet, IAC);
					TELNETSendRaw(telnet, WILL);
					TELNETSendRaw(telnet, c);
					break;
				default:
					TELNETSendRaw(telnet, IAC);
					TELNETSendRaw(telnet, WONT);
					TELNETSendRaw(telnet, c);
			}
			break;
		case STATE_DONT:
			telnet->state = STATE_TEXT;
			printf("[TELNET] DONT %s [%d]\n", TELNETGetOptionName(c), c);
			TELNETSendRaw(telnet, IAC);
			TELNETSendRaw(telnet, WONT);
			TELNETSendRaw(telnet, c);
			break;
		case STATE_SB:
			switch(c) {
				case IAC:
					telnet->state = STATE_SB_IAC;
					break;
				default:
					if(telnet->sb_write_ptr < BUFFER_SIZE) {
						telnet->sb_buf[telnet->sb_write_ptr++] = c;
					}
			}
			break;
		case STATE_SB_IAC:
			switch(c) {
				case SE:
					telnet->state = STATE_TEXT;
					TELNETProcessSB(telnet);
					break;
			}
			break;
	}
}

#ifdef VT240_NO_BUFFER
#define	BUFLEN	16384
#else
#define	BUFLEN	64
#endif

void TELNETPoll(TELNET* telnet)
{
	unsigned char buf[BUFLEN];
	if(telnet->socket == -1) {
		return;
	}

	if(!telnet->connected) {
		TELNETPollConnect(telnet);
		if(!telnet->connected) {
			return;
		}
	}

	int n = recv(telnet->socket, WINCAST buf, BUFLEN, 0);
	if(n == -1) {
#ifdef _WIN32
		if(WSAGetLastError() == WSAEWOULDBLOCK) {
#else
		if(errno == EWOULDBLOCK) {
#endif
			return;
		}

		const char* str = geterror();
		close(telnet->socket);
		telnet->socket = -1;
		TELNETRxError(telnet, "recv", str);
	} else {
		int i;
		for(i = 0; i < n; i++) {
			TELNETProcess(telnet, buf[i]);
		}
	}
}
