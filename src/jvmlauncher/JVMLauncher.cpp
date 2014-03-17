#include "JVMLauncher.h"

static std::string directory;

JVMLauncher::JVMLauncher(std::string path, std::string mainClassName, std::vector<std::string> jvmargs, std::vector<std::string> appargs, ConfigReader& config) {
    //set application home
    appHome.append(path);
    //add all jars from path
    addAllJarsFromPath(path);
	if (jars.size() == 0) {
		throw JVMLauncherException("No jar files found.");
	}
    this->mainClassName = mainClassName;
    this->config = config;
    this->jvmargs = jvmargs;
    this->appargs = appargs;
}

void JVMLauncher::addAllJarsFromPath(std::string path) {
    WIN32_FIND_DATA data;
    HANDLE hFile = FindFirstFile((path + "*.*").c_str(), &data);
	do {
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			std::string suffix = ".jar";
			std::string filename = std::string(data.cFileName);
			if (filename.size() >= suffix.size() && filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) == 0) {
				jars.push_back(filename);
			}
		}
	} while (FindNextFile(hFile, &data) != 0);
	FindClose(hFile);
}

HANDLE JVMLauncher::forkAndLaunch() {
    //Create thread pointing it at new thread entry point
    return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadEntry,
            (LPVOID) this, 0, NULL);
}

UINT WINAPI JVMLauncher::threadEntry(LPVOID param) {
    //cast param to object instance
    JVMLauncher* obj = (JVMLauncher*) param;
    //Call jvm launch
    obj->LaunchJVM();
	return 0;
}

std::string JVMLauncher::getDLLFromRegistry() {
    std::string currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
    std::string result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "RuntimeLib");
    return result;
}

std::string JVMLauncher::getJavaHomeFromRegistry() {
    std::string currentVersion = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment", "CurrentVersion");
    std::string result = getRegistryValue("SOFTWARE\\JavaSoft\\Java Runtime Environment\\" + currentVersion, "JavaHome");
    return result;
}

std::string JVMLauncher::getRegistryValue(std::string key, std::string subkey) {
    HKEY regKey;
    if (RegOpenKey(HKEY_LOCAL_MACHINE, (char*) key.c_str(), &regKey) != ERROR_SUCCESS) {
        throw JVMLauncherException("Cannot find registry key");
    }
    DWORD dwType = REG_SZ;
    char value[1024];
    DWORD value_length = 1024;
    if (RegQueryValueEx(regKey, (char*) subkey.c_str(), NULL, &dwType, (LPBYTE)&value, &value_length) != ERROR_SUCCESS) {
        throw new JVMLauncherException("Cannot find key value");
    }
    RegCloseKey(regKey);
    return value;
}

void JVMLauncher::LaunchJVM() {
    jvmDll = getDLLFromRegistry();
    javaHome = getJavaHomeFromRegistry();
    //Build library path
    std::string strJavaLibraryPath = "-Djava.library.path=";
    strJavaLibraryPath += javaHome + "\\lib" + "," + javaHome + "\\jre\\lib";
    //Add jars to classpath
    std::string strJavaClassPath = "-Djava.class.path=";
    if (jars.size() > 0) {
        for (unsigned int i = 0; i < jars.size() - 1; i++) {
            strJavaClassPath += appHome + jars[i] + ";";
        }
        strJavaClassPath += appHome + jars[jars.size() - 1];
    }
    //Configure JVM Options
	JavaVMOption options[100];
    options[0].optionString = (char*) (strJavaClassPath.c_str());
    options[1].optionString = (char*) (strJavaLibraryPath.c_str());
    options[2].optionString = (char*) "exit";
    options[2].extraInfo = (void*) *JVMLauncher::exit;
	for (unsigned int i = 3; i < jvmargs.size() + 3; i++) {
        options[i].optionString = (char*) jvmargs[(i-3)].c_str();
    }
    //Configure VM args
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.options = options;
    vm_args.nOptions = jvmargs.size() + 3;
    vm_args.ignoreUnrecognized = JNI_FALSE;
    //Load JVM.dll
    jvmDllInstance = LoadLibraryA(jvmDll.c_str());
    if (jvmDllInstance == 0) {
        throw JVMLauncherException("Cannot load jvm.dll");
    }
    //Load JVM
    jvmInstance = (CreateJavaVM) GetProcAddress(jvmDllInstance, "JNI_CreateJavaVM");
    if (jvmInstance == NULL) {
        throw JVMLauncherException("Cannot load jvm.dll");
    }
    //Create the JVM
    jint res = jvmInstance(&jvm, (void **) &jvmEnv, &vm_args);
    if (res < 0) {
        throw JVMLauncherException("Could not launch the JVM");
    }
    //Attach to main thread
    jvm->AttachCurrentThread((LPVOID*) & jvmEnv, NULL);
}

void JVMLauncher::destroyJVM() {
    jvm->DetachCurrentThread();
    jvm->DestroyJavaVM();
}

void JVMLauncher::callMainMethod() {
    jobjectArray jargs = JVMLauncherUtils::convertCLIArgs(jvmEnv, appargs);
    JVMLauncherUtils::callStaticVoidMethod(jvmEnv, mainClassName, std::string("main"), std::string("([Ljava/lang/String;)V"), jargs);
}

void JVMLauncher::callLauncherUtils() {
    jobjectArray jargs = JVMLauncherUtils::convertCLIArgs(jvmEnv, appargs);
    std::string clazzName("com/dmdirc/LauncherUtils");
    jclass clazz = JVMLauncherUtils::getClass(jvmEnv, clazzName);
    JVMLauncherUtils::registerNativeMethod(jvmEnv, clazz, std::string("setDirectory"), std::string("(Ljava/lang/String;)V"), (void*)&JVMLauncher::setDirectory);
    JVMLauncherUtils::callStaticVoidMethod(jvmEnv, clazzName, "getDirectory", "([Ljava/lang/String;)V", jargs);
}

void JVMLauncher::setDirectory(JNIEnv* env, jclass clazz, jstring string) {
    jboolean isCopy;
    directory = env->GetStringUTFChars(string, &isCopy);
}

std::string JVMLauncher::getDirectory() {
    return directory;
}

void JVMLauncher::exit(jint status) {
    cout << "JVM quitting: " << status << endl;
}
