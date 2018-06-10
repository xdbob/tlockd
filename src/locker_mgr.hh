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

#ifndef LOCKER_MGR_HH
#define LOCKER_MGR_HH

# include <utility>

# include "childs_tty.hh"

class lockerMgr : public childsTTY {
public:
	lockerMgr(int ttyfd, signalEventHandler& ev, eventManager& mgr,
		  const prog_t &base, const prog_t &locker);
	virtual ~lockerMgr();
	void lock();
	void unlock();
protected:
	virtual bool onChildDestroy(const std::shared_ptr<child> &c) override;
private:
	std::weak_ptr<child> base;
	std::weak_ptr<child> locker;
	const prog_t lock_cmd;
};

#endif /* !LOCKER_MGR_HH */
