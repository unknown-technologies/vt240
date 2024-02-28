#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <sys/wait.h>

#include "pty.h"

#define	USE_FORK

#ifdef USE_FORK
#include <sys/ioctl.h>
#else
#include <spawn.h>
#endif

void PTYInit(PTY* pty)
{
	memset(pty, 0, sizeof(PTY));
}

void PTYOpen(PTY* pty, char** argv, char** envp)
{
	pty->master = posix_openpt(O_RDWR | O_NOCTTY);
	if(pty->master == -1) {
		PTYError(pty, "posix_openpt");
		exit(1);
	}

	/* make master pty nonblocking */
	int fl = fcntl(pty->master, F_GETFL);
	if(fl == -1) {
		PTYError(pty, "fcntl");
		exit(1);
	}
	if(fcntl(pty->master, F_SETFL, fl | O_NONBLOCK)) {
		PTYError(pty, "fcntl");
		exit(1);
	}

	if(grantpt(pty->master) == -1) {
		PTYError(pty, "grantpt");
		exit(1);
	}

	if(unlockpt(pty->master) == -1) {
		PTYError(pty, "unlockpt");
		exit(1);
	}

#ifndef USE_FORK
	char* slave_filename = ptsname(pty->master);
	if(!slave_filename) {
		PTYError(pty, "ptsname");
		exit(1);
	}

	int slave = open(slave_filename, O_RDWR);
	if(slave == -1) {
		PTYError(pty, "open");
		exit(1);
	}

	struct termios ttyattrs;
	if(tcgetattr(slave, &ttyattrs)) {
		PTYError(pty, "tcgetattr");
		exit(1);
	}

	ttyattrs.c_lflag |= ECHO;

	if(tcsetattr(slave, TCSANOW, &ttyattrs)) {
		PTYError(pty, "tcsetattr");
		exit(1);
	}

	posix_spawn_file_actions_t fd_actions;
	posix_spawn_file_actions_init(&fd_actions);
	posix_spawn_file_actions_adddup2(&fd_actions, slave, STDIN_FILENO);
	posix_spawn_file_actions_adddup2(&fd_actions, slave, STDOUT_FILENO);
	posix_spawn_file_actions_adddup2(&fd_actions, slave, STDERR_FILENO);

	/* close master and original slave fd in child */
	posix_spawn_file_actions_addclose(&fd_actions, pty->master);
	posix_spawn_file_actions_addclose(&fd_actions, slave);

	posix_spawnattr_t attrs;
	posix_spawnattr_init(&attrs);
	posix_spawnattr_setflags(&attrs, POSIX_SPAWN_SETPGROUP);
	posix_spawnattr_setpgroup(&attrs, 0);
#endif

	size_t envcnt = 0;
	for(char** env = envp; *env; env++) {
		envcnt++;
	}
	envcnt += 2;

	char** env = (char**) malloc(envcnt * sizeof(char*));
	memset(env, 0, envcnt * sizeof(char*));
	char** d = env;
	for(char** p = envp; *p; p++) {
		if(strstr(*p, "LANG=")) {
			// special handling: avoid UTF-8 languages
			if(strstr(*p, "UTF-8")) {
				*(d++) = "LANG=C";
			} else {
				*(d++) = *p;
			}
		} else if(!strstr(*p, "TERM=")) {
			*(d++) = *p;
		}
	}
	*(d++) = "TERM=vt220";
	*(d++) = NULL;

#ifndef USE_FORK
	if(posix_spawnp(&pty->pid, *argv, &fd_actions, &attrs, argv, env)) {
		PTYError(pty, "posix_spawnp");
		exit(1);
	}
#else
	pid_t pid = fork();
	if(pid) {
		// parent
		pty->pid = pid;
	} else {
		// child
		if(setsid() == (pid_t) -1) {
			perror("setsid");
			fflush(stderr);
			_Exit(1);
		}

		char* slave_filename = ptsname(pty->master);
		if(!slave_filename) {
			perror("ptsname");
			fflush(stderr);
			_Exit(1);
		}

		int slave = open(slave_filename, O_RDWR);
		if(slave == -1) {
			perror("open");
			fflush(stderr);
			_Exit(1);
		}

		struct termios ttyattrs;
		if(tcgetattr(slave, &ttyattrs)) {
			perror("tcgetattr");
			fflush(stderr);
			_Exit(1);
		}

		// make sure echo is enabled
		ttyattrs.c_lflag |= ECHO;

		if(tcsetattr(slave, TCSANOW, &ttyattrs)) {
			perror("tcsetattr");
			fflush(stderr);
			_Exit(1);
		}

		if(dup2(slave, STDIN_FILENO) == -1) {
			perror("dup2");
			fflush(stderr);
			_Exit(1);
		}
		if(dup2(slave, STDOUT_FILENO) == -1) {
			perror("dup2");
			fflush(stderr);
			_Exit(1);
		}
		if(dup2(slave, STDERR_FILENO) == -1) {
			perror("dup2");
			fflush(stderr);
			_Exit(1);
		}

		// make stderr the controlling terminal
		if(ioctl(2, TIOCSCTTY, 0) == -1) {
			perror("ioctl");
			fflush(stderr);
			_Exit(1);
		}

		// close all other FDs
		for(int i = 3; i <= slave; i++)
			close(i);

		// now run the final program
		if(execve(*argv, argv, env) == -1) {
			perror("execve");
			fflush(stderr);
			_Exit(1);
		}
	}
#endif

	free(env);

#ifndef USE_FORK
	close(slave);
#endif
}

void PTYRxString(PTY* pty, const char* s)
{
	for(; *s; s++) {
		pty->rx(*s);
	}
}

void PTYRxError(PTY* pty, const char* what, const char* msg)
{
	PTYRxString(pty, what);
	PTYRxString(pty, " failed: ");
	PTYRxString(pty, msg);
	PTYRxString(pty, "\r\n");

	printf("[PTY] %s failed: %s\n", what, msg);
}

void PTYError(PTY* pty, const char* what)
{
	char* msg = strerror(errno);
	PTYRxError(pty, what, msg);
}

void PTYSend(PTY* pty, unsigned char c)
{
	if(pty->master == -1) {
		return;
	}

	ssize_t n = write(pty->master, &c, 1);
	if(n == -1) {
		PTYError(pty, "write");
		close(pty->master);
		pty->master = -1;
	}
}

#ifdef VT240_NO_BUFFER
#define	BUFLEN	16384
#else
#define	BUFLEN	64
#endif

void PTYPoll(PTY* pty)
{
	struct pollfd fds = {
		.fd = pty->master,
		.events = POLLIN | POLLHUP
	};

	unsigned char buf[BUFLEN];

	if(pty->master == -1) {
		return;
	}

	int result = poll(&fds, 1, 0);
	if(result == -1) {
		PTYError(pty, "poll");
		close(pty->master);
		pty->master = -1;
	} else if(fds.revents & POLLIN) {
		ssize_t n = read(pty->master, buf, BUFLEN);
		if(n == -1) {
			PTYError(pty, "read");
			close(pty->master);
			pty->master = -1;
		} else if(n > 0 && pty->rx) {
			for(ssize_t i = 0; i < n; i++) {
				pty->rx(buf[i]);
			}
		}
	} else if(fds.revents & POLLHUP) {
		int status;
		result = waitpid(pty->pid, &status, WNOHANG);
		if(result == -1) {
			PTYError(pty, "waitpid");
			close(pty->master);
			pty->master = -1;
		}
		if(result == 0) {
			/* process didn't exit so far */
			return;
		} else {
			char* p = (char*) buf;
			sprintf(p, "process exited with status %d\r\n", status);
			PTYRxString(pty, p);
			close(pty->master);
			pty->master = -1;
		}
	}
}

void PTYResize(PTY* pty, unsigned int width, unsigned int height)
{
	struct winsize ws = {
		.ws_col = width,
		.ws_row = height
	};

	if(ioctl(pty->master, TIOCSWINSZ, &ws) == -1) {
		printf("[PTY] failed to set console window size: %s\n", strerror(errno));
		return;
	}

	if(kill(pty->pid, SIGWINCH) == -1) {
		printf("[PTY] failed to send SIGWINCH: %s\n", strerror(errno));
	}
}
