#ifndef UPDATER_H
#define	UPDATER_H

#include <cstdlib>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "../config/ConfigReader.h"

class Updater {
public:
    Updater(ConfigReader&);
    void selfUpdate();
    void appUpdate(std::string);
    bool isUpdateWaiting();
    std::string getNewVersion();
private:
    HANDLE updateMutex;
    std::string newVersion;
    ConfigReader config;
    void deleteOldLauncher();
    void backupExistingLauncher();
    void createUpdateMutex();
    void waitForUpdaterMutex();
    void releaseUpdateMutex();
    void relaunch();
    void moveNewLauncher();
};

#endif	/* UPDATER_H */

