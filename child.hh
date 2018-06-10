#ifndef CHILD_HH
#define CHILD_HH

# include <sys/types.h>
# include <unordered_map>

# include "signal_event_handler.hh"

class child {
public:
	child(const struct termios *attr, const struct winsize *winsz,
	      const char *path, char *const argv[]);
	virtual ~child() {}

	virtual size_t read(void *buf, size_t count);
	virtual size_t write(const void *buf, size_t count);

	int getPty() const { return pty; }
	pid_t getPid() const { return pid; }

	void terminate();
private:
	int pty;
	pid_t pid;
};

#endif /* !CHILD_HH */
