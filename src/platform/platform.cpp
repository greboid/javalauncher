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
#elif WIN32
	if (MoveFileEx(oldFile.c_str(), (newFile).c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING) == 0) {
		LOGD("Unable to move file: " << strerror(errno))
		return false;
	}
	return true;
#else
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
#ifdef UNIX
	//Do something
	return "";
#elif WIN32
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	return std::string(buffer);
#else
return "";
#endif
}

std::string Platform::GetAppDataDirectory() {
#ifdef UNIX
	return "";
#elif WIN32
	PWSTR wChar;
	SHGetKnownFolderPath(FOLDERID_UserProgramFiles, 0, NULL, &wChar);
	std::wstring wpath(wChar);
	std::string path = Utils::ws2s(wpath);
	CoTaskMemFree(static_cast<LPVOID>(wChar));
	return path + "\\" + APPLICATION_NAME + "\\";
#else
return "";
#endif
}

std::string Platform::addTrailingSlash(std::string directory) {
	std::string ending;
#ifdef WIN32
	ending = "\\";
#else
	ending = "/";
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
#elif WIN32
	WIN32_FIND_DATA data;
	HANDLE hFile = FindFirstFile((addTrailingSlash(directory) + "*.*").c_str(), &data);
	std::vector<std::string> matchingFiles;
	do {
		LOGD("Checking if directory: " << std::string(data.cFileName));
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			std::string filename = std::string(data.cFileName);
			LOGD("Checking if file matches: " << filename);
			if (std::regex_match(filename, regex)) {
				LOGD("File matched");
				matchingFiles.push_back(filename);
			}
		}
	} while (FindNextFile(hFile, &data) != 0);
	FindClose(hFile);
	return matchingFiles;
#else
return std::vector<std::string>();
#endif
}

std::string Platform::launchApplicationCapturingOutput(std::string application, char** argv) {
#ifdef UNIX
	return "";
#elif WIN32
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
#else
return "";
#endif
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
#elif WIN32
	LOGD("Trying to load JVM Instancew with library: " << javaLibrary);
	HMODULE jvmDllInstance = LoadLibrary(javaLibrary.c_str());
	if (jvmDllInstance == 0) {
		LOGD("Unable to create JVM: jvmDllInstance=NULL");
		throw JVMLauncherException("Cannot create DLL instance");
	}
	//Load JVM
	CreateJavaVM jvmInstance = (CreateJavaVM)GetProcAddress(jvmDllInstance, "JNI_CreateJavaVM");
	if (jvmInstance == NULL) {
		LOGD("Unable to create JVM: jvminstance=NULL");
		throw JVMLauncherException("Cannot create Java CM");
	}
	return jvmInstance;
#else
	throw JVMLauncherException("No code for this OS.");
#endif
}

std::string Platform::getJavaDLLFromRegistry() {
#ifdef UNIX
	LOGD("Linux doesn't have a registry");
	return "";
#elif WIN32
	std::string currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
	LOGD("Registry: java current version: " << currentVersion);
	std::string result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "RuntimeLib");
	LOGD("Registry: runtime lib: " << result);
	return result;
#else
	LOGD("No code for this OS");
	return "";
#endif
}

std::string Platform::getJavaHomeFromRegistry() {
#ifdef UNIX
	LOGD("Linux doesn't have a registry");
	return "";
#elif WIN32
	std::string currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
	LOGD("Registry: java current version: " << currentVersion);
	std::string result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "JavaHome");
	LOGD("Registry: java home: " << result);
	return result;
#else
	LOGD("No code for this OS");
	return "";
#endif
}

std::string Platform::getRegistryValue(std::string key, std::string subkey) {
#ifdef UNIX
	LOGD("Linux doesn't have a registry");
	return "";
#elif WIN32
	LOGD("WIN32");
	HKEY regKey;
#if _WIN64
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)key.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &regKey) != ERROR_SUCCESS) {
#elif _WIN32
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)key.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &regKey) != ERROR_SUCCESS)  {
#endif
		throw JVMLauncherException("Cannot find registry key: " + key);
	}
	DWORD dwType = REG_SZ;
	char value[1024];
	DWORD value_length = 1024;
	if (RegQueryValueEx(regKey, (char*)subkey.c_str(), NULL, &dwType, (LPBYTE)&value, &value_length) != ERROR_SUCCESS) {
		throw new JVMLauncherException("Cannot find key value: " + key + " = " + value);
	}
	RegCloseKey(regKey);
	return value;
#else
	LOGD("No code for this OS");
	return "";
#endif
}