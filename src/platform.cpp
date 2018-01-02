#include "platform.h"
typedef jint(JNICALL* CreateJavaVM)(JavaVM**, void**, void*);

using namespace std;

Platform::Platform() {
}

void Platform::createConsole() {
#ifdef WIN32
	BOOST_LOG_TRIVIAL(debug) << "Allocationg a console.";
	AllocConsole();
	BOOST_LOG_TRIVIAL(debug) << "Attaching a console.";
	AttachConsole(GetCurrentProcessId());
	FILE *conin, *conout;
	freopen_s(&conin, "conin$", "r", stdin);
	freopen_s(&conout, "conout$", "w", stdout);
	freopen_s(&conout, "conout$", "w", stderr);
#endif
}

void Platform::launchApplication(string application, string arguments) {
#ifdef WIN32
	BOOST_LOG_TRIVIAL(debug) << "Launching: " << application << " Args: " << arguments;
	ShellExecute(NULL, LPSTR("open"), LPSTR(application.c_str()), LPSTR(arguments.c_str()), NULL, SW_SHOWNORMAL);
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
	BOOST_LOG_TRIVIAL(debug) << "Moving File: " << oldFile << " => " << newFile;
	if (MoveFileEx(oldFile.c_str(), (newFile).c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0) {
		DWORD dw = GetLastError();
		BOOST_LOG_TRIVIAL(debug) << "Unable to move file: " << dw << ": " << strerror(dw);
		return false;
	}
	return true;
#else
return true;
#endif
}
void Platform::platformInit() {
#ifdef WIN32
	SetProcessDPIAware();
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

std::string Platform::addTrailingSlash(std::string directory) {
	std::string ending;
#ifdef WIN32
	ending = "\\";
#else
	ending = "/";
#endif
	if (0 != directory.compare(directory.length() - ending.length(), ending.length(), ending)) {
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
		BOOST_LOG_TRIVIAL(debug) << "Checking if directory: " << std::string(data.cFileName);
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			std::string filename = std::string(data.cFileName);
			BOOST_LOG_TRIVIAL(debug) << "Checking if file matches: " << filename;
			if (std::regex_match(filename, regex)) {
				BOOST_LOG_TRIVIAL(debug) << "File matched";
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
	BOOST_LOG_TRIVIAL(debug) << "Trying to load JVM Instancew with library: " << javaLibrary;
	HMODULE jvmDllInstance = LoadLibrary(javaLibrary.c_str());
	if (jvmDllInstance == 0) {
		BOOST_LOG_TRIVIAL(debug) << "Unable to create JVM: jvmDllInstance=NULL";
		throw JVMLauncherException("Cannot create DLL instance");
	}
	//Load JVM
	CreateJavaVM jvmInstance = (CreateJavaVM)GetProcAddress(jvmDllInstance, "JNI_CreateJavaVM");
	if (jvmInstance == NULL) {
		BOOST_LOG_TRIVIAL(debug) << "Unable to create JVM: jvminstance=NULL";
		throw JVMLauncherException("Cannot create Java CM");
	}
	return jvmInstance;
#else
	throw JVMLauncherException("No code for this OS.");
#endif
}

std::string Platform::getJavaDLLFromRegistry() {
#ifdef UNIX
	BOOST_LOG_TRIVIAL(debug) << "Linux doesn't have a registry");
	return "";
#elif WIN32
  std::string currentVersion;
  try {
    currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
  }
  catch (JVMLauncherException& ex) {
    try {
      BOOST_LOG_TRIVIAL(debug) << "Registry: CurrentVersion: Java Runtime Environment key not found, trying JRE.";
      currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\JRE", "CurrentVersion");
    }
    catch (JVMLauncherException& ex1) {
      BOOST_LOG_TRIVIAL(debug) << "Registry: CurrentVersion: JRE key not found.  No JRE found.";
      currentVersion = "";
    }
  }
	BOOST_LOG_TRIVIAL(debug) << "Registry: java current version: " << currentVersion;
  std::string result;
  if (currentVersion != "") {
    try {
      BOOST_LOG_TRIVIAL(debug) << "Registry: RuntimeLib: First Try";
      result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\"+currentVersion, "RuntimeLib");
    }
    catch (JVMLauncherException& ex) {
      BOOST_LOG_TRIVIAL(debug) << "Registry: RuntimeLib: Java Runtime Environment key not found, trying JRE.";
      try {
        result = getRegistryValue("SOFTWARE\\JavaSoft\\JRE\\"+currentVersion, "RuntimeLib");
      }
      catch (JVMLauncherException& ex1) {
        BOOST_LOG_TRIVIAL(debug) << "Registry: RuntimeLib: JRE key not found.  No JRE found.";
        result = "";
      }
    }
    BOOST_LOG_TRIVIAL(debug) << "Registry: runtime lib: " << result;
  }
  else {
    result = "";
  }
	return result;
#else
	BOOST_LOG_TRIVIAL(debug) << "No code for this OS";
	return "";
#endif
}

std::string Platform::getJavaHomeFromRegistry() {
#ifdef UNIX
	BOOST_LOG_TRIVIAL(debug) << "Linux doesn't have a registry");
	return "";
#elif WIN32
  std::string currentVersion;
  try {
    currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
  }
  catch (JVMLauncherException& ex) {
    BOOST_LOG_TRIVIAL(debug) << "Registry: CurrentVersion: Java Runtime Environment key not found, trying JRE.";
    try {
      currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\JRE", "CurrentVersion");
    }
    catch (JVMLauncherException& ex1) {
      BOOST_LOG_TRIVIAL(debug) << "Registry: CurrentVersion: JRE key not found.  No JRE found.";
      currentVersion = "";
    }
  }
	BOOST_LOG_TRIVIAL(debug) << "Registry: java current version: " << currentVersion;
  std::string result;
  if (currentVersion != "") {
    try {
      currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "JavaHome");
    }
    catch (JVMLauncherException& ex) {
      BOOST_LOG_TRIVIAL(debug) << "Registry: JavaHome: Java Runtime Environment key not found, trying JRE.";
      try {
        result = getRegistryValue("SOFTWARE\\JavaSoft\\JRE\\"+currentVersion, "JavaHome");
      }
      catch (JVMLauncherException& ex1) {
        BOOST_LOG_TRIVIAL(debug) << "Registry: JavaHome: JRE key not found.  No JRE found.";
        result = "";
      }
    }
    BOOST_LOG_TRIVIAL(debug) << "Registry: java home: " << result;
  }
  else {
    result = "";
  }
	return result;
#else
	BOOST_LOG_TRIVIAL(debug) << "No code for this OS";
	return "";ege
#endif
}

std::string Platform::getRegistryValue(std::string key, std::string subkey) {
#ifdef UNIX
	BOOST_LOG_TRIVIAL(debug) << "Linux doesn't have a registry";
	return "";
#elif WIN32
	BOOST_LOG_TRIVIAL(debug) << "WIN32";
#if _WIN64
  BOOST_LOG_TRIVIAL(debug) << "WIN64";
  HKEY regKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)key.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &regKey) != ERROR_SUCCESS) {
#elif _WIN32
  BOOST_LOG_TRIVIAL(debug) << "_WIN32";
  HKEY regKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, (char*)key.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &regKey) != ERROR_SUCCESS)  {
#endif
		throw JVMLauncherException("Cannot find registry key: " + key);
	}
	DWORD dwType = REG_SZ;
	char value[1024];
	DWORD value_length = 1024;
	if (RegQueryValueEx(regKey, (char*)subkey.c_str(), NULL, &dwType, (LPBYTE)&value, &value_length) != ERROR_SUCCESS) {
		throw JVMLauncherException("Cannot find key "+ key + " subkey: " + subkey);
	}
	RegCloseKey(regKey);
	return value;
#else
	BOOST_LOG_TRIVIAL(debug) << "No code for this OS";
	return "";
#endif
}
