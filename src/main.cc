#include <cstdio>
#include <signal.h>
#include <exception>

#include "event_manager.hh"
#include "signal_event_handler.hh"
#include "childs_tty.hh"
#include "io.hh"

#include "locker_mgr.hh"

static bool exitSignal(const struct signalfd_siginfo &info) {
	(void) info;
	return false;
}

int main(int argc, const char *const argv[]) {
	(void) argc;

	eventManager mgr;
	signalEventHandler sigs(mgr);

	for (const auto s : { SIGINT, SIGTERM, SIGSTOP})
		sigs.registerCallback(s, exitSignal);

	const char * const tst[] = { "vlock", "-c", nullptr };
	const char *vl = "vlock";
	lockerMgr::prog_t lckcmd(vl, tst);
	lockerMgr ct(0, sigs, mgr, {argv[1], argv + 1}, lckcmd);
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
	}

	return 0;
}
