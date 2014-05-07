#ifndef PLATFORM_H
#define	PLATFORM_H

#include "../log4z/log4z.h"
#include "../utils/utils.h"
#ifdef UNIX
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#endif
#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#endif
#include <fstream>


class Platform {
public:
	static void createConsole();
	static void launchApplication(std::string application, char** argv);
	static bool moveFile(std::string oldFile, std::string newFile);
	static bool deleteFileIfExists(std::string file);
	static bool deleteFile(std::string file);
	static std::string getExePath();
	static void disableFolderVirtualisation();
	static std::string GetAppDataDirectory();
private:
	Platform();
};

#endif	/* PLATFORM_H */
