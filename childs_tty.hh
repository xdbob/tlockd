#ifndef CHILDS_TTY_HH
#define CHILDS_TTY_HH

# include <termios.h>
# include <pty.h>

# include "child_manager.hh"

class childsTTY : public childManager {
public:
	childsTTY(int ttyfd, signalEventHandler& ev, eventManager& mgr);
	virtual ~childsTTY();

	virtual void spawnChild(const char *path, char *const argv[]) override;
protected:
	virtual bool handle_child_event(const struct epoll_event &e,
					std::shared_ptr<child> c) override;
private:
	std::weak_ptr<child> focus;
	int ttyfd;
	const struct termios attrs;
	struct winsize winp;
	eventManager& mgr;
	bool handle_tty(const struct epoll_event &e);
	unsigned char buf[4096];
};

#endif /* !CHILD_TTY_HH */
