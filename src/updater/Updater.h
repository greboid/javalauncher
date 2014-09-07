#ifndef UPDATER_H
#define	UPDATER_H

#include "log4z/log4z.h"
#include "platform/platform.h"
#include "platform/mutex.h"
#include "config/ConfigReader.h"
#include <string>
#include <iostream>
#include <fstream>

class Updater {
public:
    Updater(ConfigReader&);
    bool doUpdate(std::string);
    bool isUpdateWaiting();
    std::string getNewVersion();
    void relaunch(char** argv);
private:
    Mutex updateMutex;
    std::string newVersion;
    ConfigReader config;
	void deleteOldLauncher();
	void getAndLockMutex();
	int updateLauncher(std::string, std::string);
	int updateApplication(std::string, std::string);
};

#endif	/* UPDATER_H */

