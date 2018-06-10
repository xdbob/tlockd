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

#include <cstddef>
#include <cerrno>

#include "error.hh"
#include "io.hh"

namespace unistd {
#include <unistd.h>
}

static inline bool try_again() {
	return errno == EAGAIN
	    || errno == EINTR;
}

size_t io::write(int fd, const void *buf, size_t count) {
	ssize_t r;

	while ((r = unistd::write(fd, buf, count)) < 0 && try_again())
		continue;

	if (r < 0)
		throw make_system_error("write");

	return r;
}

size_t io::read(int fd, void *buf, size_t count) {
	ssize_t r;

	while ((r = unistd::read(fd, buf, count)) < 0 && try_again())
		continue;

	if (r < 0)
		throw make_system_error("read");

	return r;
}
