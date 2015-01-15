#ifndef SINGLEINSTANCE_H
#define	SINGLEINSTANCE_H

#include "log4z.h"
#include "ConfigReader.h"
#include "mutex.h"
#include <cstdlib>
#include <iostream>
#include <string>

class SingleInstance {
public:
    SingleInstance(ConfigReader&);
    virtual ~SingleInstance();
    bool getCanStart();
    void stopped();
private:
    Mutex instanceMutex;
    ConfigReader config;
};

#endif	/* SINGLEINSTANCE_H */