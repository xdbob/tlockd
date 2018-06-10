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

	size_t pos = head + this->count;
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

	size_t to_cpy = head + count > this->count ? size - head : count;
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
