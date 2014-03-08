#include "ConfigReader.h"

ConfigReader::ConfigReader() {
	this->name = "launcher.ini";
}

ConfigReader::ConfigReader(std::string name) {
    this->name = name;
}

ConfigReader::~ConfigReader() {
}

std::string ConfigReader::getStringValue(std::string key, std::string defaultValue) {
    return "";
}

bool ConfigReader::getBoolValue(std::string key, bool defaultValue) {
	return defaultValue;
}
