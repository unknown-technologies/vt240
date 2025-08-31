#ifndef __TELNET_H__
#define __TELNET_H__

typedef struct {
	int		socket;
	int		connected;
	int		state;

	/* tx ringbuffer */
	unsigned char*	buf;
	int		read_ptr;
	int		write_ptr;
	int		count;

	/* sb buffer */
	unsigned char*	sb_buf;
	int		sb_write_ptr;

	void		(*rx)(unsigned char c);
} TELNET;

void TELNETInit(TELNET* telnet);
void TELNETConnect(TELNET* telnet, const char* hostname, int port);
void TELNETDisconnect(TELNET* telnet);
void TELNETSend(TELNET* telnet, unsigned char c);
void TELNETBreak(TELNET* telnet);
void TELNETSendRaw(TELNET* telnet, unsigned char c);
void TELNETPoll(TELNET* telnet);

#endif
