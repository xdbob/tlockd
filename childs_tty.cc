#include <err.h>

#include "child.hh"
#include "childs_tty.hh"
#include "error.hh"
#include "io.hh"

static struct termios getAttrs(int fd) {
	struct termios attrs;
	if (tcgetattr(fd, &attrs) < 0)
		throw make_system_error("tcgetattr");
	return attrs;
}

childsTTY::childsTTY(int ttyfd, signalEventHandler& ev, eventManager& mgr) :
		childManager(ev, mgr), ttyfd(ttyfd),
		attrs(getAttrs(ttyfd)), mgr(mgr) {
	struct termios tmp = attrs;
	cfmakeraw(&tmp);

	if (tcsetattr(ttyfd, TCSANOW, &tmp) < 0)
		throw make_system_error("tcsetattr");

	mgr.registerEvent(ttyfd, [this](const auto& e){ return handle_tty(e); }, true);
}

childsTTY::~childsTTY() {
	if (tcsetattr(ttyfd, TCSANOW, &attrs))
		warn("tcsetattr");
	mgr.unregisterEvent(ttyfd);
}

bool childsTTY::handle_tty(const struct epoll_event &e) {
	if (focus.expired() || e.events == EPOLLHUP)
		return true;
	focus.lock()->write(buf, io::read(ttyfd, buf, sizeof (buf)));
	return true;
}

std::shared_ptr<child> childsTTY::createChild(const prog_t &cmd) {
	return std::make_shared<child>(&attrs, &winp, cmd.first, cmd.second);
}

void childsTTY::spawnChild(const prog_t &cmd) {
	auto c = createChild(cmd);

	if (focus.expired())
		focus = c;

	registerChild(std::move(c));
}

bool childsTTY::handle_child_event(const struct epoll_event &e, std::shared_ptr<child> c) {
	if (e.events == EPOLLHUP)
		return true;

	auto r = c->read(buf, sizeof (buf));
	if (!focus.expired() && focus.lock() == c)
		io::write(ttyfd, buf, r);

	return true;
}
