#include "mutex.h"

Mutex::Mutex() {
	instance = NULL;
}

bool Mutex::init(std::string name)
{
#ifdef UNIX
	return pthread_mutex_init(instance, NULL);
#endif
#ifdef WIN32
	instance = CreateMutex(NULL, TRUE, TEXT(name.c_str()));
	if (instance && GetLastError() == ERROR_ALREADY_EXISTS) {
		return FALSE;
	}
	return TRUE;
#endif
	return TRUE;
}

int Mutex::lock()
{
#ifdef UNIX
	return pthread_mutex_lock(mutex);
#endif
#ifdef WIN32
	return (WaitForSingleObject(instance, INFINITE) == WAIT_FAILED ? 1 : 0);
#endif
	return -1;
}

int Mutex::unlock()
{
#ifdef UNIX
	return pthread_mutex_unlock(mutex);
#endif
#ifdef WIN32
	bool value = ReleaseMutex(instance) == 0;
	CloseHandle(instance);
	return value;
#endif
	return -1;
}