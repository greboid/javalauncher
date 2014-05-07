#ifndef MUTEX_H
#define	MUTEX_H

#ifdef UNIX
#include <pthread.h>
#endif
#ifdef WIN32
#include <windows.h>
#include <process.h>
#endif
#include <string>

#ifdef UNIX
#define MUTEX pthread_mutex_t
#endif
#ifdef WIN32
#define MUTEX HANDLE
#endif

class Mutex {
public:
	bool init(std::string name);
	int lock();
	int unlock();
private:
	MUTEX instance;
};

#endif	/* MUTEX_H */