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

#include <cstring>

#include "ringbuffer.hh"

ringbuffer::ringbuffer(size_t size) :
	size(size), head(0), count(0), data(new unsigned char[size])
{}

ringbuffer::~ringbuffer() {
	delete[] data;
}

void ringbuffer::push(const void *buf, size_t count) {
	auto src = reinterpret_cast<const unsigned char *>(buf);
	auto dst = data;

	if (count > size) {
		src += count - size;
		count = size;
	}

	size_t pos = (head + this->count) % size;
	size_t to_cpy = pos + count > size ? size - pos : count;
	memcpy(dst + pos, src, to_cpy);
	if (to_cpy < count)
		memcpy(dst, src + to_cpy, count - to_cpy);

	this->count += count;
	if (this->count > this->size)
		head = (head + this->count - size) % size;
	this->count %= size;
}

size_t ringbuffer::read(void *dst, size_t count) const {
	auto d = reinterpret_cast<unsigned char *>(dst);
	auto src = data;
	if (count > this->count)
		count = this->count;

	size_t to_cpy = head + count > this->size ? size - head : count;
	memcpy(d, src + head, to_cpy);
	if (to_cpy < count)
		memcpy(d + to_cpy, src, count - to_cpy);

	return count;
}

size_t ringbuffer::pop(void *dst, size_t count) {
	count = read(dst, count);

	this->count -= count;
	head = (head + count) % size;

	return count;
}

void ringbuffer::setSize(size_t sz) {
	// XXX: do something smart
	unsigned char *new_data = new unsigned char[sz];

	if (count < sz) {
		head = (head + count - sz) % size;
		this->count = sz;
	}

	this->count = pop(new_data, sz);
	head = 0;
	size = sz;
	delete[] data;
	data = new_data;
}
