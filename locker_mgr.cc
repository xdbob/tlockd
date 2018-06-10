#include "locker_mgr.hh"

lockerMgr::lockerMgr(int ttyfd, signalEventHandler& ev, eventManager& mgr,
		     const prog_t &base, const prog_t &lock) :
		childsTTY(ttyfd, ev, mgr), lock_cmd(lock) {
	childsTTY::spawnChild(base);
	this->base = getFocus();
}

lockerMgr::~lockerMgr() {}

bool lockerMgr::onChildDestroy(const std::shared_ptr<child> &c) {
	if (base.lock() == c)
		return false;
	if (locker.lock() == c)
		setFocus(base);
	return true;
}

void lockerMgr::lock() {
	if (auto x = locker.lock()) {
		locker.reset();
		x->terminate();
	}
	auto c = createChild(lock_cmd);
	setFocus(c);
	locker = c;
	registerChild(std::move(c));
}

void lockerMgr::unlock() {
	setFocus(base);
	if (auto x = locker.lock()) {
		locker.reset();
		x->terminate();
	}
}
