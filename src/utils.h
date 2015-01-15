#ifndef UTILS_H
#define	UTILS_H

#include "log4z.h"
#include "ConfigDefaults.h"
#include "ConfigReader.h"
#include "platform.h"
#include <string>
#include <stdlib.h>
#include <vector>
#include <regex>
#include <iostream>

#define BUFSIZE 4096 

class Utils {
public:
	static std::vector<std::string> mergeVectors(std::vector<std::string>, std::vector<std::string>);
	static std::vector<std::string> arrayToVector(int, char**);
	static std::string vectorToString(std::vector<std::string>);
	static std::vector<std::string> splitString(std::string, std::string);
	static std::string getExePathAndName();
	static std::string getExePath();
	static std::string getExeName();
	static std::string ws2s(std::wstring);
	static void addMatchingFilesToExistingVector(std::vector<std::string>&, std::string, std::regex);
	static std::vector<std::string> addMatchingFilesToVector(std::string, std::regex);
	static std::string launchAppReturnOutput(std::string, char** argv);
private:
	Utils();
protected:
};

#endif	/* UTILS_H */
