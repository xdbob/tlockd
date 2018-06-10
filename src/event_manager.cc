#include <unistd.h>

#include "error.hh"
#include "event_manager.hh"
#include "utils.hh"

static constexpr size_t epollMax = 10;

eventManager::eventManager(size_t residentEvents) : resident(residentEvents) {
	if ((epollfd = epoll_create1(0)) < 0)
		throw make_system_error("epoll_create1");
}

void eventManager::registerEvent(int fd, callback_t clbk, bool resident) {
	struct epoll_event ev = {
		.events = EPOLLIN | EPOLLHUP | EPOLLERR,
		.data = { .fd = fd },
	};
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw make_system_error("epoll_ctl(ADD)");
	callbacks[fd] = clbk;
	if (resident)
		++this->resident;
}

void eventManager::unregisterEvent(int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, nullptr);
	callbacks.erase(fd);
}

eventManager::~eventManager() {
	close(epollfd);
}

void eventManager::run() {
	struct epoll_event evs[epollMax];
	while (callbacks.size() > resident) {
		int nfds = epoll_wait(epollfd, evs, array_size(evs), -1);
		if (nfds < 0)
			throw make_system_error("epoll_wait");

		for (int i = 0; i < nfds; ++i) {
			auto fct = callbacks[evs[i].data.fd];
			if (evs[i].events == EPOLLHUP)
				unregisterEvent(evs[i].data.fd);
			if (!fct)
				continue;
			if (!fct(evs[i]))
				return;
		}
	}
}
