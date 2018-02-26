/*
* keyboard.c - read commands
*
*
* Entry points:
*
*	k_donext(cmd)
*	char *cmd;
*		Arrange that cmd will be done next.
*
*	k_finish()
*		Close down the keyboard manager.
*
*	int k_getch()
*		Return the next character of the current command.
*
*	k_init()
*		Initialize the keyboard manager.
*
*	char k_lastcmd()
*		Return the first letter in the last command.
*
*	k_newcmd()
*		Prepare for reading a new command.
*
*	k_redo()
*		Redo the last buffer-change command.
*
*	int k_keyin()
*		Get a character from the keyboard.
*
*
* External procedure calls:
*
*	int b_changed()					.. file Bman.c
*		Tell if the buffer was changed by the last command.
*
*	b_newcmd(bit)					.. file Bman.c
*	int bit;
*		Inform the buffer module that a new command is starting and tell
*		whether it is from the keyboard (bit = 1) or not (bit = 0).
*
*	s_keyboard(bit)					.. file Sman.c
*	int bit;
*		Inform the screen module whether the next input character is
*		from the keyboard (bit = 1) or not (bit = 0).
*
*	s_savemsg(msg, count)				.. file Sman.c
*	char *msg;
*	int count;
*		Format msg and save it for the next screen refreshing.
*/

#ifdef TERMIOS
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
static struct termios oldt;
#else
#ifdef CONIO
#include <conio.h>
#include <signal.h>
#else
#include <sgtty.h>
static struct sgttyb oldt;
#endif
#endif
#include <string.h>

#include "s.h"

#define CMD_MAX 500		/* longest command that can be redone */

extern void s_savemsg(), s_keyboard(), b_newcmd();
extern int b_changed();
int k_keyin();
void k_flip();

static char
	change[CMD_MAX+2],	/* most recent buffer-change command */
	cmd_last,		/* first letter in the last command */
	command[CMD_MAX+2],	/* accumulates the current command */
	*cmd_ptr = command,	/* next location in command */
	pushed[CMD_MAX],	/* pushed-back command */
	*push_ptr = pushed;	/* next location in pushed */

/* k_donext - push a command back on the input stream */
void k_donext(cmd)
char *cmd;
{
	int cmd_size;
	char *s;

	cmd_size = strlen(cmd);
	if (push_ptr - pushed + cmd_size > CMD_MAX) {
		s_savemsg("Pushed commands are too long.", 0);
		UNKNOWN;
	} else if (cmd_size > 0) {
		/* copy cmd to pushed[] in reverse order */
		for (s = cmd + cmd_size - 1; s >= cmd; --s)
			*push_ptr++ = *s;
		s_keyboard(0);
	}
}

/* k_finish - close down the keyboard manager */
void k_finish()
{
	k_flip();
}

/* k_getch - get a character of the command */
int k_getch()
{
	int ch;

	/* get pushed character (preferably) or read keyboard */
	/* use logical AND operation with octal 0177 to strip the parity bit */
	ch = (push_ptr > pushed) ? *(--push_ptr) : k_keyin() & 0177;
	/* remember character if there is room */
	if (cmd_ptr <= command + CMD_MAX)
		*cmd_ptr++ = ch;
	s_keyboard(push_ptr == pushed);
	return(ch);
}

/* k_init - initialize the keyboard manager */
void k_init()
{
	k_flip();
}

/* k_lastcmd - get first letter of the last command */
char k_lastcmd()
{
	return(cmd_last);
}

/* k_newcmd - start a new command */
void k_newcmd()
{
	char *s;

	*cmd_ptr = '\0';
	/* remember first letter of the old command */
	for (s = command; *s != '\0' && !isalpha(*s); ++s)
		;
	cmd_last = *s;
	/* if the old command changed the buffer, remember it */
	if (b_changed())
		strcpy(change, command);
	cmd_ptr = command;		/* prepare to collect the new command */
	b_newcmd(push_ptr == pushed);	/* mark buffer "unchanged" */
}

/* k_redo - redo the last buffer-change command */
void k_redo()
{
	if (strlen(change) > CMD_MAX) {
		s_savemsg("Cannot redo commands longer than %d characters.",
			CMD_MAX);
		change[0] = '\0';
	}
	if (change[0] == '\0')
		UNKNOWN;
	else
		k_donext(change);
}

/* keyboard input mode */
static int k_raw = 0;

/*	
* k_keyin - get a character from the keyboard
* Hide system dependent differences in keyboard input
*/

int k_keyin()
{
#ifdef CONIO
	if (k_raw) {
		return getch();
	} else {
		return getchar();
	}
#else
	return getchar();
#endif
}

/*	
* k_flip  - toggle keyboard input to and from noecho-raw mode  (UNIX)
* Normally:
*	1. typed characters are echoed back to the terminal and
*	2. input characters are buffered until a complete line
*	   has been received.
* Flipping to noecho-raw mode suspends all such input processing.
*/

void k_flip()
{
#ifdef TERMIOS
	struct termios newt;
#else
#ifndef CONIO
	struct sgttyb newt;
#endif
#endif

	if (!k_raw) {
		k_raw = 1;
#ifdef CONIO
		/* Stop SIGINT (<CTRL-C>) detection */
		/* Keyboard reads during screen redraw kills raw input */
		signal(SIGINT, SIG_IGN);
#else
#ifdef TERMIOS
		ioctl(0, TCGETS, &oldt);
		ioctl(0, TCGETS, &newt);
		newt.c_lflag &= ~(ISIG|ICANON|ECHO);
		newt.c_iflag &= ~(INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF);
		newt.c_oflag &= ~OPOST;
		newt.c_cc[VMIN]  = 1;
		newt.c_cc[VTIME] = 0;
		ioctl(0, TCSETSW, &newt);
#else
		ioctl(0, TIOCGETP, &oldt);
		ioctl(0, TIOCGETP, &newt);
		newt.sg_flags |= RAW;
		newt.sg_flags &= ~ECHO;
		ioctl(0, TIOCSETP, &newt);
#endif
#endif
	} else {
		k_raw = 0;
#ifdef CONIO
		/* normal SIGINT handling */
		signal(SIGINT, SIG_DFL);
#else
#ifdef TERMIOS
		ioctl(0, TCSETSW, &oldt);
#else
		ioctl(0, TIOCSETP, &oldt);
#endif
#endif
	}
}
