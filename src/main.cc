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

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <signal.h>
#include <unistd.h>

#include "child_tty.hh"
#include "event_manager.hh"
#include "error.hh"
#include "io.hh"
#include "locker_mgr.hh"
#include "signal_event_handler.hh"

static bool exitSignal(const struct signalfd_siginfo &info) {
	(void) info;
	return false;
}

static void registerPid() {
	char pid[10];
	snprintf(pid, sizeof (pid), "%u", getpid());

	if (setenv("TLOCKD_PID", pid, 1) < 0)
		throw make_system_error("setenv");
}

int main(int argc, const char *const argv[]) {
	eventManager mgr;
	signalEventHandler sigs(mgr);

	for (const auto s : { SIGINT, SIGTERM, SIGSTOP})
		sigs.registerCallback(s, exitSignal);

	const char *shell = getenv("SHELL");
	if (!shell)
		shell = "/bin/sh";
	const char * const shellargv[] = { shell, nullptr };

	lockerMgr::prog_t lckcmd;
	if (argc < 2) {
		static const char * const deflock[] = { "vlock", "-c", nullptr };
		lckcmd = { deflock[0], deflock };
	} else {
		lckcmd = { argv[1], argv + 1 };
	}

	registerPid();

	lockerMgr ct(0, sigs, mgr, {shell, shellargv}, lckcmd);
	sigs.registerCallback(SIGUSR1, [&ct](const auto &x) {
			(void) x;
			ct.lock();
			return true;
		});
	sigs.registerCallback(SIGUSR2, [&ct](const auto &x) {
			(void) x;
			ct.unlock();
			return true;
		});

	try {
		mgr.run();
	} catch (std::exception &e) {
		fprintf(stderr, "error: %s", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
