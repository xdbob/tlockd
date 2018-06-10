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
