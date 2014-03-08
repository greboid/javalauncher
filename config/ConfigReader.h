#ifndef CONFIGREADER_H
#define	CONFIGREADER_H

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(std::string);
    virtual ~ConfigReader();
    std::string getStringValue(std::string, std::string);
	bool getBoolValue(std::string, bool);
private:
    std::map<std::string, std::string> settings;
    std::string name;
    void init();
    bool parseBoolean(const std::string*);
};

#endif	/* CONFIGREADER_H */

