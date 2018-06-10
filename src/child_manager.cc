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

#include <signal.h>
#include <sys/wait.h>

#include "child.hh"
#include "child_manager.hh"

childManager::childManager(signalEventHandler& ev, eventManager& mgr) :
	mgr(mgr), ev(ev) {
	ev.registerCallback(SIGCHLD, [this](const auto &x) {
			return handle_signal(x);
		});
}

childManager::~childManager() {
	ev.unregisterCallback(SIGCHLD);
	for (const auto& c : childs_pid) {
		mgr.unregisterEvent(c.second->getPty());
		c.second->terminate();
	}
}

bool childManager::onChildDestroy(const std::shared_ptr<child> &c) {
	(void) c;
	return true;
}

bool childManager::handle_signal(const struct signalfd_siginfo &info) {
	int s;
	waitpid(info.ssi_pid, &s, 0);

	const auto &c = childs_pid.find(info.ssi_pid);
	auto lastref = c->second;

	mgr.unregisterEvent(c->second->getPty());

	childs_pty.erase(c->second->getPty());
	childs_pid.erase(c);

	return onChildDestroy(lastref);
}

bool childManager::child_handler(const struct epoll_event &e) {
	return handle_child_event(e, childs_pty.at(e.data.fd).lock());
}

bool childManager::handle_child_event(const struct epoll_event &ev,
				      std::shared_ptr<child> c) {
	(void) ev;
	(void) c;
	return true;
}

void childManager::registerChild(std::shared_ptr<child> &&c) {
	childs_pid[c->getPid()] = c;
	childs_pty[c->getPty()] = c;
	mgr.registerEvent(c->getPty(), [this](const auto& e) {
			return child_handler(e);
		});
}

void childManager::spawnChild(const prog_t &cmd) {
	registerChild(createChild(cmd));
}

std::shared_ptr<child> childManager::createChild(const prog_t &cmd) {
	return std::make_shared<child>(nullptr, nullptr, cmd.first, cmd.second);
}

void childManager::applyAll(std::function<void(std::shared_ptr<child> &c)> f) {
	for (auto& c : childs_pid)
		f(c.second);
}
