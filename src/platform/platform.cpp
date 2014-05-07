#include "platform.h"

using namespace std;

Platform::Platform() {
}

void Platform::createConsole() {
#ifdef WIN32
	LOGD("Allocationg a console.");
	AllocConsole();
	LOGD("Attaching a console.");
	AttachConsole(GetCurrentProcessId());
	FILE *conin, *conout;
	freopen_s(&conin, "conin$", "r", stdin);
	freopen_s(&conout, "conout$", "w", stdout);
	freopen_s(&conout, "conout$", "w", stderr);
#endif
}

void Platform::launchApplication(string application, char** arguments) {
#ifdef WIN32
	ShellExecute(NULL, LPSTR("open"), LPSTR(application.c_str()), LPSTR(arguments), NULL, SW_SHOWNORMAL);
#endif
}

bool Platform::moveFile(std::string oldFile, std::string newFile) {
#ifdef UNIX
	char buf[BUFSIZ];
	size_t size;

	int source = open(oldFile.c_str(), O_RDONLY, 0);
	int dest = open(newFile.c_str(), O_WRONLY | O_CREAT, 0644);

	while ((size = read(source, buf, BUFSIZ)) > 0) {
		write(dest, buf, size);
	}

	close(source);
	close(dest);
	return TRUE;
#endif
#ifdef WIN32
	if (MoveFileEx(oldFile.c_str(), (newFile + ".old").c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) == 0) {
		perror("Unable to move file");
		return false;
	}
	return true;
#endif
}

bool Platform::deleteFileIfExists(string file) {
	std::ifstream iFile(file.c_str());
	if (iFile.good()) {
		iFile.close();
		return deleteFile(file);
	}
	return true;
}

bool Platform::deleteFile(string file) {
	if (remove(file.c_str()) != 0) {
		perror("Failed to delete the old launcher");
		return false;
	}
	return true;
}

string Platform::getExePath() {
#ifdef WIN32
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
#endif
}

void Platform::disableFolderVirtualisation() {
#ifdef WIN32
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
		if (GetLastError() != ERROR_INVALID_PARAMETER) {
			return;
		}
		CloseHandle(hToken);
	}
#endif
}

std::string Platform::GetAppDataDirectory() {
#ifdef WIN32
	PWSTR wChar;
	SHGetKnownFolderPath(FOLDERID_UserProgramFiles, 0, NULL, &wChar);
	std::wstring wpath(wChar);
	std::string path = Utils::ws2s(wpath);
	CoTaskMemFree(static_cast<LPVOID>(wChar));
	return path + "\\" + APPLICATION_NAME + "\\";
#endif
}

std::string Platform::addTrailingSlash(std::string directory) {
#ifdef UNIX
	std::string endinf = "/";
#endif
#ifdef WIN32
	std::string ending = "\\";
#endif
	if (0 != directory.compare(directory.length() - ending.length(), ending.length(), ending)) {
		LOGD("Adding trailing slash.");
		return directory + ending;
	}
	return directory;
}

std::vector<std::string> Platform::listDirectory(std::string directory) {
	return listDirectory(directory, std::regex(".*"));
}

std::vector<std::string> Platform::listDirectory(std::string directory, std::regex regex) {
#ifdef WIN32
	WIN32_FIND_DATA data;
	HANDLE hFile = FindFirstFile((addTrailingSlash(directory) + "*.*").c_str(), &data);
	std::vector<std::string> matchingFiles;
	do {
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			LOGD("Checking if file matches: " << data.cFileName);
			std::string filename = std::string(data.cFileName);
			if (std::regex_match(filename, regex)) {
				LOGD("File matched");
				matchingFiles.push_back(filename);
			}
		}
	} while (FindNextFile(hFile, &data) != 0);
	FindClose(hFile);
	return matchingFiles;
#endif
}