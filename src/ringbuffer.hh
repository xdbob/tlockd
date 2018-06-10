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

#ifndef RINGBUFFER_HH
#define RINGBUFFER_HH

# include <cstddef>

class ringbuffer {
public:
	ringbuffer(size_t size);
	~ringbuffer();

	void push(const void *buf, size_t count);
	size_t pop(void *dst, size_t count);
	size_t read(void *dst, size_t count) const;

	size_t getCount() const { return count; }
	size_t getSize() const { return size; }
	void setSize(size_t sz);
private:
	size_t size;
	size_t head;
	size_t count;
	unsigned char *data;
};

#endif /* !RINGBUFFER_HH */
