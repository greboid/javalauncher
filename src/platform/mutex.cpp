#include "mutex.h"

Mutex::Mutex() {
	instance = NULL;
}

bool Mutex::init(std::string name)
{
#ifdef UNIX
	return pthread_mutex_init(instance, NULL);
#elseif WIN32
	instance = CreateMutex(NULL, true, TEXT(name.c_str()));
	if (instance && GetLastError() == ERROR_ALREADY_EXISTS) {
		return FALSE;
	}
	return true;
#else
	return true;
#endif
}

int Mutex::lock()
{
#ifdef UNIX
	return pthread_mutex_lock(mutex);
#elseif WIN32
	return (WaitForSingleObject(instance, INFINITE) == WAIT_FAILED ? 1 : 0);
#else
	return -1;
#endif
}

int Mutex::unlock()
{
#ifdef UNIX
	return pthread_mutex_unlock(mutex);
#elseif WIN32
	bool value = ReleaseMutex(instance) == 0;
	CloseHandle(instance);
	return value;
#else
	return -1;
#endif
}
