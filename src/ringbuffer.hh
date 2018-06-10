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
