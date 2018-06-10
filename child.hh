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
	virtual ~child() {}

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
