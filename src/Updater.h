#ifndef UPDATER_H
#define	UPDATER_H

#include "Logger.h"
#include "platform.h"
#include "mutex.h"
#include "ConfigReader.h"
#include <string>
#include <iostream>
#include <fstream>

class Updater {
public:
	Updater(ConfigReader&);
	void moveApplicationUpdates();
    bool doUpdate(std::string);
    bool isUpdateWaiting();
    std::string getNewVersion();
    void relaunch(std::string args);
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

