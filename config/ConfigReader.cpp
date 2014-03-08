#include "ConfigReader.h"

using namespace std;

ConfigReader::ConfigReader() {
	this->name = "launcher.ini";
	init();
}

ConfigReader::ConfigReader(string name) {
    this->name = name;
    init();
}

void ConfigReader::init() {
    string line;
    ifstream configFile ((char*) name.c_str(), ios::in);
    if (configFile.is_open()) {
        string key;
        string value;
        while(getline(configFile, line)) {
            int pos = line.find("=");
            if(pos != string::npos) {
                key = line.substr(0, pos );
                value = line.substr(pos + 1);
                settings[key] = value;
            }
        }
    }
    configFile.close();
}

ConfigReader::~ConfigReader() {
}

string ConfigReader::getStringValue(string key, string defaultValue) {
    map<string,string>::const_iterator search = settings.find(key);
    if (search == settings.end() ) {
    	return defaultValue;
    } else {
        return search->second;
    }
}

bool ConfigReader::getBoolValue(string key, bool defaultValue) {
    map<string,string>::const_iterator search = settings.find(key);
    if (search == settings.end() ) {
    	return defaultValue;
    } else {
        return parseBoolean(&search->second);
    }
}

bool ConfigReader::parseBoolean(const std::string *str) {
    return (char*) str == "true" || (char*) str == "yes" || (char*) str == "on" || (char*) str == "1";
}
