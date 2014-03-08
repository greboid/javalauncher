#ifndef CONFIGREADER_H
#define	CONFIGREADER_H

#include <cstdlib>
#include <string>

class ConfigReader {
public:
    ConfigReader();
    ConfigReader(std::string);
    virtual ~ConfigReader();
    std::string getStringValue(std::string, std::string);
	bool getBoolValue(std::string, bool);
private:
    std::string name;
};

#endif	/* CONFIGREADER_H */

