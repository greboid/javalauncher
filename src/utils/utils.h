#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include <shlobj.h>
#include "../config/ConfigDefaults.h"
#include "../config/ConfigReader.h"

class Utils {
    public:
        static std::vector<std::string> mergeVectors(std::vector<std::string>, std::vector<std::string>);
        static std::vector<std::string> arrayToVector(int, char**);
        static std::vector<std::string> splitString(std::string, std::string);
        static std::string getExePathAndName();
        static std::string getExePath();
        static std::string getExeName();
        static void disableFolderVirtualisation();
        static std::string ws2s(std::wstring s);
		static std::string GetAppDataDirectory();
		static void addMatchingFilesToExistingVector(std::vector<std::string>, std::string, std::string);
		static std::vector<std::string> addMatchingFilesToVector(std::string, std::string);
    private:
        Utils();
    protected:
};

#endif	/* UTILS_H */
