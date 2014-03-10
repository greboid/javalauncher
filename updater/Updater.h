#ifndef UPDATER_H
#define	UPDATER_H

#include <cstdlib>
#include <windows.h>
#include "../config/ConfigReader.h"

class Updater {
public:
    Updater(ConfigReader&);
private:
    ConfigReader config;
    void MoveRunningExecutable();

};

#endif	/* UPDATER_H */

