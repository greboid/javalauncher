#ifndef CONFIGREADER_H
#define	CONFIGREADER_H

#include "log4z/log4z.h"
#include "utils/utils.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
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
    bool parseBoolean(const std::string*);
};

#endif	/* CONFIGREADER_H */

