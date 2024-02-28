#ifndef __PTY_H__
#define __PTY_H__

#include <sys/types.h>

typedef struct {
	int	master;
	pid_t	pid;

	void	(*rx)(unsigned char c);
} PTY;

void	PTYInit(PTY* pty);
void	PTYOpen(PTY* pty, char** argv, char** envp);
void	PTYSend(PTY* pty, unsigned char c);
void	PTYPoll(PTY* pty);
void	PTYResize(PTY* pty, unsigned int width, unsigned int height);

void	PTYRxString(PTY* pty, const char* s);
void	PTYRxError(PTY* pty, const char* what, const char* msg);
void	PTYError(PTY* pty, const char* what);

#endif
