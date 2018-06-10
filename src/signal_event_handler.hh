#ifndef SIGNAL_EVENT_HANDLER_HH
#define SIGNAL_EVENT_HANDLER_HH

#include <signal.h>
#include <sys/signalfd.h>

# include "event_manager.hh"

void reset_signals();

class signalEventHandler {
public:
	using callback_t = std::function<bool(const struct signalfd_siginfo &info)>;
	signalEventHandler(eventManager &e);
	~signalEventHandler();

	void registerCallback(int signo, callback_t clbk);
	void unregisterCallback(int signo);
private:
	static int sigfd;
	static size_t refcount;
	eventManager &e;
	std::unordered_map<int, callback_t> handlers;
};

#endif /* !SIGNAL_EVENT_HANDLER_HH */
