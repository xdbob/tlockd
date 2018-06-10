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
