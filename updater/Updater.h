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
    void doUpdate();
    bool isUpdateWaiting();
    std::string getNewVersion();
private:
    std::string newVersion;
    ConfigReader config;
    void deleteOldLauncher();
    void backupExistingLauncher();
};

#endif	/* UPDATER_H */

