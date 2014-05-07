#ifndef PLATFORM_H
#define	PLATFORM_H

#include "../log4z/log4z.h"
#ifdef UNIX
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#endif
#ifdef WIN32
#include <windows.h>
#endif

class Platform {
public:
	static void createConsole();
	static void launchApplication(std::string application, char** argv);
	static bool moveFile(std::string oldFile, std::string newFile);
private:
	Platform();
};

#endif	/* PLATFORM_H */

