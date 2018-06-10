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

#ifndef EVENT_MANAGER_HH
#define EVENT_MANAGER_HH

# include <sys/epoll.h>
# include <functional>
# include <unordered_map>

class eventManager {
public:
	using callback_t = std::function<bool(const struct epoll_event&)>;

	eventManager(size_t residentEvents = 0);
	virtual ~eventManager();

	void registerEvent(int fd, callback_t clbk, bool resident = false);
	void unregisterEvent(int fd);

	void run();

	size_t getResident() const { return resident; }
	void setResident(size_t r) { resident = r; }
private:
	int epollfd;
	size_t resident;
	std::unordered_map<int, callback_t> callbacks;
};

#endif /* !EVENT_MANAGER_HH */
