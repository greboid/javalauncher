#ifndef UPDATER_H
#define	UPDATER_H

#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include "../config/ConfigReader.h"

class Updater {
public:
    Updater(ConfigReader&);
    bool doUpdate(std::string);
    bool isUpdateWaiting();
    std::string getNewVersion();
    void relaunch();
private:
    HANDLE updateMutex;
    std::string newVersion;
    ConfigReader config;
    void deleteOldLauncher();
    void backupExistingLauncher();
    void createUpdateMutex();
    void waitForUpdaterMutex();
    void releaseUpdateMutex();
    bool moveNewLauncher(std::string, std::string);
	int updateLauncher(std::string, std::string);
	int updateApplication(std::string, std::string);
};

#endif	/* UPDATER_H */

