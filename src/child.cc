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
		int tmp;
		waitpid(pid, &tmp, 0);
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
