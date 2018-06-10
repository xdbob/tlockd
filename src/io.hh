#ifndef IO_HH
#define IO_HH

# include <cstddef>

namespace io {
	size_t write(int fd, const void *buf, size_t count);
	size_t read(int fd, void *buf, size_t count);
}

#endif
