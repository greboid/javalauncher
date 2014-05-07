#include "../log4z/log4z.h"
#include "ConfigReader.h"

using namespace std;

ConfigReader::ConfigReader() {
	this->name = (Utils::getExeName() + std::string(".ini"));
	LOGD("Config file: " << this->name);
	init();
}

ConfigReader::ConfigReader(string name) {
    this->name = name;
	LOGD("Config file: " << this->name);
    init();
}

void ConfigReader::init() {
    string line;
    ifstream configFile ((char*) (Utils::getExePath() +"//" + name).c_str(), ios::in);
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
		LOGD("Config Value (string): " << key << " : " << search->second);
        return search->second;
    }
}

bool ConfigReader::getBoolValue(string key, bool defaultValue) {
    map<string,string>::const_iterator search = settings.find(key);
    if (search == settings.end() ) {
    	return defaultValue;
    } else {
		LOGD("Config Value (bool): " << key << " : " << &search->second);
        return parseBoolean(&search->second);
    }
}

std::vector<std::string> ConfigReader::getVectorValue(string key, std::vector<std::string> defaultValue) {
    map<string,string>::const_iterator search = settings.find(key);
    if (search == settings.end() ) {
    	return defaultValue;
    }
	LOGD("Config Value (vector): " << key << " : " << search->second);
    return Utils::splitString(search->second, ",");
}

bool ConfigReader::parseBoolean(const std::string *str) {
	return strcmp((char*)str, "true") || strcmp((char*)str, "yes") || strcmp((char*)str, "on") || strcmp((char*)str, "1");
}
