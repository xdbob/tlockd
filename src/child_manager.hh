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
	virtual bool onChildDestroy(const std::shared_ptr<child> &c);
	void registerChild(std::shared_ptr<child> &&c);
	void applyAll(std::function<void(std::shared_ptr<child> &c)> f);
private:
	bool child_handler(const struct epoll_event &e);
	std::unordered_map<pid_t, std::shared_ptr<child>> childs_pid;
	std::unordered_map<int, std::weak_ptr<child>> childs_pty;
	bool handle_signal(const struct signalfd_siginfo &info);
	eventManager& mgr;
	signalEventHandler& ev;
};

#endif /* !CHILD_MANAGER_HH */
