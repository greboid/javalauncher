#include "utils.h"

Utils::Utils() {
}

std::string Utils::getExePathAndName() {
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    return std::string(buffer);
}

std::string Utils::getExeName() {
    std::string fullPath = getExePathAndName();
    std::string::size_type pos = std::string(fullPath).find_last_of("\\/");
    return std::string(fullPath).substr(pos+1);
}

std::string Utils::getExePath() {
    std::string fullPath = getExePathAndName();
    std::string::size_type pos = std::string(fullPath).find_last_of("\\/");
    return std::string(fullPath).substr( 0, pos);
}

std::vector<std::string> Utils::mergeVectors(std::vector<std::string> vector1, std::vector<std::string> vector2) {
    std::vector <std::string> result (vector1.size() + vector2.size());
    int index = 0;
    for(unsigned int i = 0; i < vector1.size(); i++) {
        result[index] = vector1[i];
        index++;
    }
    for(unsigned int i = 0; i < vector2.size(); i++) {
        result[index] = vector2[i];
        index++;
    }
    return result;
}

std::vector<std::string> Utils::arrayToVector(int size, char** array) {
    std::vector <std::string> result (size);
    for (int i = 0; i < size; i++) {
        result[i] = array[i];
    }
    return result;
}

std::vector<std::string> Utils::splitString(std::string input, std::string delimiter) {
      unsigned start = 0;
      unsigned end;
      std::vector<std::string> result;

      while( (end = input.find(delimiter, start)) != std::string::npos )
      {
            result.push_back(input.substr(start, end-start));
            start = end + delimiter.length();
      }
      result.push_back(input.substr(start));
      return result;
}


void Utils::disableFolderVirtualisation() {
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
        if (GetLastError() != ERROR_INVALID_PARAMETER) {
            return;
        }
        CloseHandle(hToken);
    }
}

std::string Utils::ws2s(std::wstring s) {
	int len;
	int slength = (int)s.length();
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}

std::string Utils::GetAppDataDirectory() {
	PWSTR wChar;
	SHGetKnownFolderPath(FOLDERID_UserProgramFiles, 0, NULL, &wChar);
	std::wstring wpath(wChar);
	std::string path = Utils::ws2s(wpath);
	CoTaskMemFree(static_cast<LPVOID>(wChar));
	return path + "\\" + APPLICATION_NAME + "\\";
}
