#ifndef CONFIGREADER_H
#define	CONFIGREADER_H

#include <cstdlib>
#include <string>

class ConfigReader {
public:
    ConfigReader();
    virtual ~ConfigReader();
    std::string getStringValue(std::string, std::string);
    bool getBoolValue(std::string, bool);
private:

};

#endif	/* CONFIGREADER_H */

