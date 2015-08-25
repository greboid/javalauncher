#ifndef SINGLEINSTANCE_H
#define	SINGLEINSTANCE_H

#include "Logger.h"
#include "mutex.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

class SingleInstance {
public:
    SingleInstance(boost::program_options::variables_map&);
    virtual ~SingleInstance();
    bool getCanStart();
    void stopped();
private:
    Mutex instanceMutex;
	boost::program_options::variables_map config;
};

#endif	/* SINGLEINSTANCE_H */