#ifndef PLATFORM_H
#define	PLATFORM_H

#include "log4z/log4z.h"
#include "utils/utils.h"
#include "jvmlauncher/JVMLauncherException.cpp"
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
#include <jni.h>
#include <vector>
#include <regex>
#include <fstream>


class Platform {
public:
	typedef jint(JNICALL* CreateJavaVM)(JavaVM**, void**, void*);
	static void createConsole();
	static void launchApplication(std::string application, char** argv);
	static bool moveFile(std::string oldFile, std::string newFile);
	static bool deleteFileIfExists(std::string file);
	static bool deleteFile(std::string file);
	static std::string getExePath();
	static void disableFolderVirtualisation();
	static std::string GetAppDataDirectory();
	static std::string addTrailingSlash(std::string directory);
	static std::vector<std::string> listDirectory(std::string directory);
	static std::vector<std::string> listDirectory(std::string directory, std::regex regex);
	static std::string launchApplicationCapturingOutput(std::string application, char** argv);
	static CreateJavaVM getJVMInstance(std::string javaLibrary);
private:
	Platform();
};

#endif	/* PLATFORM_H */

