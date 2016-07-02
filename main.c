/*
 * Dead Simple Linux Input Event Key Binder
 *
 * CLEVO/Notebook WMI Hotkey Driver Companion
 *
 * Copyright (C) 2016 Guillermo A. Amaral B. <g@maral.me>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/input.h>

#include "config.h"

#define EVENT_ARRAY_SIZE 32

static int s_working = 1;

static int grab_input(const char *);
static int process_events(int fd);
static int run_script(int type, int code, int value);
static void exit_handler(int);

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s EVDEV\n", argv[0]);
		return EXIT_SUCCESS;
	}

	if (grab_input(argv[1]))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int
grab_input(const char *node)
{
	int fd;
	int rc;

	if (-1 == (fd = open(node, O_RDONLY))) {
		perror("evbind");
		return -1;
	}

	rc = process_events(fd);

	close(fd);

	return rc;
}

int
process_events(int fd)
{
	struct input_event event[EVENT_ARRAY_SIZE];
	struct sigaction exit_action;
	int i, c;
	int bytes_read;

	/*
	 * Catch signals used to break out of event loop.
	 */
	exit_action.sa_handler = exit_handler;
	sigemptyset(&exit_action.sa_mask);
	exit_action.sa_flags = 0;

	sigaction(SIGHUP, &exit_action, NULL);
	sigaction(SIGINT, &exit_action, NULL);
	sigaction(SIGQUIT, &exit_action, NULL);

	/*
	 * Event Loop
	 */

	do {
		bytes_read = read(fd, event, sizeof(struct input_event) * EVENT_ARRAY_SIZE);

		c = bytes_read / (ssize_t) sizeof(struct input_event);

		for (i = 0; i < c; ++i)
			run_script(event[i].type, event[i].code, event[i].value);
	} while (s_working && c > 0);

	return 0;
}

int
run_script(int type, int code, int value)
{
	char code_str[6];
	const char *type_str;
	pid_t exec;

	switch (type) {
	case EV_KEY:  type_str = "KEY"; break;
	case EV_MSC:  type_str = "MSC"; break;
	default: return -1;
	}

	snprintf(code_str, sizeof(code_str), "%d", code);

	if (0 == (exec = fork())) {
		execl("/bin/sh", "sh", DEFAULT_SCRIPT,
		    type_str, code_str, value ? "1" : "0", (char *)NULL);
	}
	else if (-1 == exec) {
		perror("evbind");
		return -1;
	}

	return 0;
}

void
exit_handler(int signo)
{
	(void)signo; /* UNUSED */
	s_working = 0;
}

