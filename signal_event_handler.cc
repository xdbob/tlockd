#include <signal.h>
#include <sys/signalfd.h>
#include <unistd.h>

#include "signal_event_handler.hh"
#include "error.hh"

int signalEventHandler::sigfd;
size_t signalEventHandler::refcount;
sigset_t signalEventHandler::oldset;

static void clean_sigfd(int fd, const sigset_t *mask) {
	close(fd);
	sigprocmask(SIG_BLOCK, mask, nullptr);
}

signalEventHandler::signalEventHandler(eventManager &e) : e(e){
	if (!refcount) {
		sigset_t mask;
		if (sigfillset(&mask) < 0)
			throw make_system_error("sigfillset");

		if (sigprocmask(SIG_BLOCK, &mask, &oldset) < 0)
			throw make_system_error("sigprocmask");

		sigfd = signalfd(-1, &mask, SFD_CLOEXEC);
		if (sigfd < 0) {
			auto err = make_system_error("signalfd");
			clean_sigfd(-1, &oldset);
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
					clbk = handlers.at(fdsi.ssi_signo);
				} catch (std::out_of_range &) {
					fprintf(stderr, "Unhandled signal: %d\n",
							fdsi.ssi_signo);
					return true;
				}
				return clbk(fdsi);
			}, true);
}

signalEventHandler::~signalEventHandler() {
	if (!--refcount)
		clean_sigfd(sigfd, &oldset);
	e.unregisterEvent(sigfd);
}

void signalEventHandler::registerCallback(int signo, callback_t clbk) {
	handlers[signo] = clbk;
}

void signalEventHandler::unregisterCallback(int signo) {
	handlers.erase(signo);
}
