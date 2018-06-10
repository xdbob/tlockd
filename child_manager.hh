#ifndef CHILD_MANAGER_HH
#define CHILD_MANAGER_HH

# include <unordered_map>
# include <memory>

# include "child.hh"
# include "signal_event_handler.hh"

class childManager {
public:
	using prog_t = std::pair<const char *, const char * const *>;
	childManager(signalEventHandler& ev, eventManager& mgr);
	virtual ~childManager();
	virtual void spawnChild(const prog_t &cmd);
protected:
	virtual std::shared_ptr<child> createChild(const prog_t &cmd);
	virtual bool handle_child_event(const struct epoll_event &e,
					std::shared_ptr<child> c);
	void registerChild(std::shared_ptr<child> &&c);
private:
	bool child_handler(const struct epoll_event &e);
	std::unordered_map<pid_t, std::shared_ptr<child>> childs_pid;
	std::unordered_map<int, std::weak_ptr<child>> childs_pty;
	bool handle_signal(const struct signalfd_siginfo &info);
	eventManager& mgr;
	signalEventHandler& ev;
};

#endif /* !CHILD_MANAGER_HH */
