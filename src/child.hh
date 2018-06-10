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

#ifndef CHILD_HH
#define CHILD_HH

# include <memory>
# include <sys/types.h>
# include <unordered_map>

# include "signal_event_handler.hh"
# include "ringbuffer.hh"

class child {
public:
	child(const struct termios *attr, const struct winsize *winsz,
	      const char *path, const char *const argv[], bool buffered = true);
	~child();

	size_t read(void *buf, size_t count);
	size_t write(const void *buf, size_t count);
	size_t getBufCount() const;
	void getBuffer(void *buf) const;

	int getPty() const { return pty; }
	pid_t getPid() const { return pid; }

	void terminate();
private:
	int pty;
	pid_t pid;
	std::unique_ptr<ringbuffer> rb;
};

#endif /* !CHILD_HH */
