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
