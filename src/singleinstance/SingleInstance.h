#ifndef SINGLEINSTANCE_H
#define	SINGLEINSTANCE_H

#include "log4z/log4z.h"
#include "config/ConfigReader.h"
#include "platform/mutex.h"
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