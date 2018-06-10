/*
 * This file is part of tlockd.
 *
 * Copyright (C) 2018 Antoine Damhet
 *
 * tlockd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tlockd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tlockd.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <fcntl.h>
#include <pty.h>
#include <signal.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "child.hh"
#include "error.hh"
#include "io.hh"

#include "signal_event_handler.hh"

child::child(const struct termios *attr, const struct winsize *winp,
	     const char *path, const char *const argv[], bool buffered) {
	pid = forkpty(&pty, NULL, attr, winp);
	if (pid < 0)
		throw make_system_error("forkpty");

	if (!pid) {
		reset_signals();
		execvp(path, (char **)argv);
		_exit(1);
	}

	if (fcntl(pty, F_SETFD, FD_CLOEXEC) < 0) {
		auto err = make_system_error("fcntl(SETFD, CLOEXEC)");
		close(pty);
		terminate();
		waitpid(pid, nullptr, 0);
		throw err;
	}
	if (buffered && winp) {
		assert(winp->ws_col && winp->ws_row);
		rb = std::make_unique<ringbuffer>(winp->ws_col * winp->ws_row);
	}
}

void child::terminate() {
	kill(pid, SIGTERM);
}

size_t child::read(void *buf, size_t count) {
	auto r = io::read(pty, buf, count);
	if (rb)
		rb->push(buf, r);
	return r;
}

size_t child::write(const void *buf, size_t count) {
	return io::write(pty, buf, count);
}

size_t child::getBufCount() const {
	if (!rb)
		return 0;
	return rb->getCount();
}

void child::getBuffer(void *buf) const {
	rb->read(buf, rb->getCount());
}
