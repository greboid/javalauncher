#ifndef CONFIGREADER_H
#define	CONFIGREADER_H

#include "log4z.h"
#include "utils.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(std::string);
    virtual ~ConfigReader();
    std::string getStringValue(std::string, std::string);
	bool getBoolValue(std::string, bool);
	std::vector<std::string> getVectorValue(std::string, std::vector<std::string>);
private:
    std::map<std::string, std::string> settings;
    std::string name;
    void init();
    bool parseBoolean(std::string);
};

#endif	/* CONFIGREADER_H */
