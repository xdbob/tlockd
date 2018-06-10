#ifndef LOCKER_MGR_HH
#define LOCKER_MGR_HH

# include <utility>

# include "childs_tty.hh"

class lockerMgr : public childsTTY {
public:
	lockerMgr(int ttyfd, signalEventHandler& ev, eventManager& mgr,
		  const prog_t &base, const prog_t &locker);
	virtual ~lockerMgr();
	void lock();
	void unlock();
protected:
	virtual bool onChildDestroy(const std::shared_ptr<child> &c) override;
private:
	std::weak_ptr<child> base;
	std::weak_ptr<child> locker;
	const prog_t lock_cmd;
};

#endif /* !LOCKER_MGR_HH */
