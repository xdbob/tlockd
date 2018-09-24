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

#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "signal_event_handler.hh"
#include "error.hh"

int signalEventHandler::sigfd;
size_t signalEventHandler::refcount;
static sigset_t oldset;

void reset_signals() {
	sigprocmask(SIG_SETMASK, &oldset, nullptr);
}

signalEventHandler::signalEventHandler(eventManager &e) : e(e){
	if (!refcount) {
		sigset_t mask;
		if (sigfillset(&mask) < 0)
			throw make_system_error("sigfillset");

		if (sigprocmask(SIG_SETMASK, &mask, &oldset) < 0)
			throw make_system_error("sigprocmask");

		sigfd = signalfd(-1, &mask, SFD_CLOEXEC);
		if (sigfd < 0) {
			auto err = make_system_error("signalfd");
			reset_signals();
			throw err;
		}
	}

	++refcount;

	e.registerEvent(sigfd,
			[this](const struct epoll_event& ev) {
				struct signalfd_siginfo fdsi;
				if (read(ev.data.fd, &fdsi, sizeof (fdsi)) != sizeof (fdsi))
					throw make_system_error("read(signafd)");
				callback_t clbk;
				try {
					clbk = handlers.at(static_cast<pid_t>(fdsi.ssi_signo));
				} catch (std::out_of_range &) {
					fprintf(stderr, "Unhandled signal: %d\n",
							fdsi.ssi_signo);
					return true;
				}
				return clbk(fdsi);
			}, true);
}

signalEventHandler::~signalEventHandler() {
	if (!--refcount) {
		close(sigfd);
		reset_signals();
	}
	e.unregisterEvent(sigfd);
}

void signalEventHandler::registerCallback(int signo, callback_t clbk) {
	handlers[signo] = clbk;
}

void signalEventHandler::unregisterCallback(int signo) {
	handlers.erase(signo);
}
