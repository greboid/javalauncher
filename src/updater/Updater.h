#ifndef UPDATER_H
#define	UPDATER_H

#include "../log4z/log4z.h"
#include "../platform/mutex.h"
#include "../config/ConfigReader.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <conio.h>

class Updater {
public:
    Updater(ConfigReader&);
    bool doUpdate(std::string);
    bool isUpdateWaiting();
    std::string getNewVersion();
    void relaunch();
private:
    Mutex updateMutex;
    std::string newVersion;
    ConfigReader config;
    void deleteOldLauncher();
    void backupExistingLauncher();
    bool moveNewLauncher(std::string, std::string);
	int updateLauncher(std::string, std::string);
	int updateApplication(std::string, std::string);
};

#endif	/* UPDATER_H */

