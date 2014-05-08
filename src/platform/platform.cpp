#include "platform.h"
typedef jint(JNICALL* CreateJavaVM)(JavaVM**, void**, void*);

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
	return true;
#endif
#ifdef WIN32
	if (MoveFileEx(oldFile.c_str(), (newFile + ".old").c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) == 0) {
		perror("Unable to move file");
		return false;
	}
	return true;
#endif
return true;
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
#ifdef UNIX
	//Do something
	return "";
#endif
#ifdef WIN32
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
#endif
return "";
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
#ifdef UNIX
	return "";
#endif
#ifdef WIN32
	PWSTR wChar;
	SHGetKnownFolderPath(FOLDERID_UserProgramFiles, 0, NULL, &wChar);
	std::wstring wpath(wChar);
	std::string path = Utils::ws2s(wpath);
	CoTaskMemFree(static_cast<LPVOID>(wChar));
	return path + "\\" + APPLICATION_NAME + "\\";
#endif
return "";
}

std::string Platform::addTrailingSlash(std::string directory) {
	std::string ending;
#ifdef UNIX
	ending = "/";
#endif
#ifdef WIN32
	ending = "\\";
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
#ifdef UNIX
	return std::vector<std::string>();
#endif
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
return std::vector<std::string>();
}

std::string Platform::launchApplicationCapturingOutput(std::string application, char** argv) {
#ifdef UNIX
	return "";
#endif
#ifdef WIN32
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
	BOOL bSuccess;
	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	std::string commandLine = application + " --LAUNCHER_VERSION";
	bSuccess = CreateProcess(LPSTR(application.c_str()), LPSTR(commandLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
	if (!bSuccess) {
		return "0";
	}

	DWORD dwRead;
	bSuccess = FALSE;
	std::string output;
	if (WaitForSingleObject(piProcInfo.hProcess, 1000) == WAIT_TIMEOUT) {
		TerminateProcess(piProcInfo.hProcess, 1);
	}
	else {
    CHAR chBuf[BUFSIZE];
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
#endif
return "";
}

CreateJavaVM Platform::getJVMInstance(std::string javaLibrary) {
#ifdef UNIX
	void* jvmDllInstance = dlopen(javaLibrary.c_str());
	if (jvmDllInstance == 0) {
		throw JVMLauncherException("Cannot load jvm.dll");
	}
	CreateJavaVM jvmInstance = (CreateJavaVM)dlsym(jvmDllInstance, "JNI_CreateJavaVM");
	if (jvmInstance == NULL) {
		throw JVMLauncherException("Cannot load jvm.dll");
	}
	return jvmInstance;
#elseif WIN32
	HMODULE jvmDllInstance = LoadLibraryA(javaLibrary.c_str());
	if (jvmDllInstance == 0) {
		throw JVMLauncherException("Cannot load jvm.dll");
	}
	//Load JVM
	CreateJavaVM jvmInstance = (CreateJavaVM)GetProcAddress(jvmDllInstance, "JNI_CreateJavaVM");
	if (jvmInstance == NULL) {
		throw JVMLauncherException("Cannot load jvm.dll");
	}
	return jvmInstance;
#else
	throw JVMLauncherException("Cannot load jvm.dll");
#endif
}

std::string Platform::getJavaDLLFromRegistry() {
#ifdef UNIX
	return "";
#elseif WIN32
	std::string currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
	std::string result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "RuntimeLib");
	return result;
#else
	return "";
#endif
	return "";
}

std::string Platform::getJavaHomeFromRegistry() {
#ifdef UNIX
	return "";
#elseif WIN32
	std::string currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
	std::string result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "JavaHome");
	return result;
#else
	return "";
#endif
	return "";
}

std::string Platform::getRegistryValue(std::string key, std::string subkey) {
#ifdef UNIX
	return "";
#elseif WIN32
	HKEY regKey;
	if (RegOpenKey(HKEY_LOCAL_MACHINE, (char*)key.c_str(), &regKey) != ERROR_SUCCESS) {
		throw JVMLauncherException("Cannot find registry key");
	}
	DWORD dwType = REG_SZ;
	char value[1024];
	DWORD value_length = 1024;
	if (RegQueryValueEx(regKey, (char*)subkey.c_str(), NULL, &dwType, (LPBYTE)&value, &value_length) != ERROR_SUCCESS) {
		throw new JVMLauncherException("Cannot find key value");
	}
	RegCloseKey(regKey);
	return value;
#else
	return "";
#endif
	return "";
}