#include <cstdio>
#include <signal.h>
#include <exception>

#include "event_manager.hh"
#include "signal_event_handler.hh"
#include "childs_tty.hh"
#include "io.hh"

static bool exitSignal(const struct signalfd_siginfo &info) {
	(void) info;
	return false;
}

int main(int argc, char *const argv[]) {
	(void) argc;

	eventManager mgr;
	signalEventHandler sigs(mgr);
	sigs.registerCallback(SIGINT, exitSignal);

	childsTTY ct(0, sigs, mgr);
	ct.spawnChild(argv[1], argv + 1);

	try {
		mgr.run();
	} catch (std::exception &e) {
		fprintf(stderr, "error: %s", e.what());
	}

	return 0;
}
