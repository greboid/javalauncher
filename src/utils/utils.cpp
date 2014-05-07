#include "utils.h"

using namespace std;

Utils::Utils() {
}

std::string Utils::getExePathAndName() {
	return Platform::getExePath();
}

std::string Utils::getExeName() {
	std::string fullPath = getExePathAndName();
	std::string::size_type pos = std::string(fullPath).find_last_of("\\/");
	return std::string(fullPath).substr(pos + 1);
}

std::string Utils::getExePath() {
	std::string fullPath = getExePathAndName();
	std::string::size_type pos = std::string(fullPath).find_last_of("\\/");
	return std::string(fullPath).substr(0, pos);
}

std::vector<std::string> Utils::mergeVectors(std::vector<std::string> vector1, std::vector<std::string> vector2) {
	std::vector <std::string> result(vector1.size() + vector2.size());
	int index = 0;
	for (unsigned int i = 0; i < vector1.size(); i++) {
		result[index] = vector1[i];
		index++;
	}
	for (unsigned int i = 0; i < vector2.size(); i++) {
		result[index] = vector2[i];
		index++;
	}
	return result;
}

std::vector<std::string> Utils::arrayToVector(int size, char** array) {
	std::vector <std::string> result(size);
	for (int i = 0; i < size; i++) {
		result[i] = array[i];
	}
	return result;
}

std::vector<std::string> Utils::splitString(std::string input, std::string delimiter) {
	unsigned start = 0;
	unsigned end;
	std::vector<std::string> result;

	while ((end = input.find(delimiter, start)) != std::string::npos)
	{
		result.push_back(input.substr(start, end - start));
		start = end + delimiter.length();
	}
	result.push_back(input.substr(start));
	return result;
}

std::string Utils::ws2s(std::wstring s) {
	return string(s.begin(), s.end());
}

void Utils::addMatchingFilesToExistingVector(std::vector<std::string>& files, std::string path, std::regex regex) {
	std::vector<std::string> newFiles = addMatchingFilesToVector(path, regex);
	files = Platform::listDirectory(path, regex);
}

std::vector<std::string> Utils::addMatchingFilesToVector(std::string path, std::regex regex) {
	return Platform::listDirectory(path, regex);
}

std::string Utils::launchAppReturnOutput(std::string executable) {
	HANDLE g_hChildStd_OUT_Rd;
	HANDLE g_hChildStd_OUT_Wr;
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)) {
		return "-1";
	}
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		return "-1";
	}
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	std::string commandLine = executable + " --LAUNCHER_VERSION";
	bSuccess = CreateProcess(LPSTR(executable.c_str()), LPSTR(commandLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
	if (!bSuccess) {
		return "0";
	}

	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::string output;
	if (WaitForSingleObject(piProcInfo.hProcess, 1000) == WAIT_TIMEOUT) {
		TerminateProcess(piProcInfo.hProcess, 1);
	}
	else {
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0){
			return "0";
		}
		for (unsigned int i = 0; i <= dwRead; i++) {
			if (chBuf[i] == '\r' || chBuf[i] == '\n' || chBuf[i] == '\0') {
				output = std::string(chBuf, i);
				break;
			}
		}
	}
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);
	return output;
}