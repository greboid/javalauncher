#ifndef SINGLEINSTANCE_H
#define	SINGLEINSTANCE_H

#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <string>
#include "../config/ConfigReader.h"

class SingleInstance {
public:
    SingleInstance(ConfigReader*);
    virtual ~SingleInstance();
    bool getCanStart();
    void stopped();
private:
    HANDLE instanceMutex;
    ConfigReader* config;
};

#endif	/* SINGLEINSTANCE_H */

