#ifndef EVENT_MANAGER_HH
#define EVENT_MANAGER_HH

# include <sys/epoll.h>
# include <functional>
# include <unordered_map>

class eventManager {
public:
	using callback_t = std::function<bool(const struct epoll_event&)>;

	eventManager(size_t residentEvents = 0);
	virtual ~eventManager();

	void registerEvent(int fd, callback_t clbk, bool resident = false);
	void unregisterEvent(int fd);

	void run();

	size_t getResident() const { return resident; }
	void setResident(size_t r) { resident = r; }
private:
	int epollfd;
	size_t resident;
	std::unordered_map<int, callback_t> callbacks;
};

#endif /* !EVENT_MANAGER_HH */
