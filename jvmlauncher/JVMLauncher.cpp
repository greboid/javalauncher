#include "JVMLauncher.h"

JVMLauncher::JVMLauncher(std::string path, std::string mainClassName, std::vector<std::string> jvmargs, std::vector<std::string> appargs, ConfigReader* config) {
    //set application home
    appHome.append(path);
    //add all jars from path
    addAllJarsFromPath(path);
    this->mainClassName = mainClassName;
    this->config = config;
    this->jvmargs = jvmargs;
    this->appargs = appargs;
}

void JVMLauncher::addAllJarsFromPath(std::string path) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir((char*) path.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            std::string suffix = ".jar";
            std::string filename = std::string(ent->d_name);
            if (filename.size() >= suffix.size() && filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) == 0) {
                jars.push_back(filename);
            }
        }
        closedir (dir);
    }
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

void JVMLauncher::disableFolderVirtualisation() {
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) {
        DWORD tokenInfoVal = 0;
        if (GetLastError() != ERROR_INVALID_PARAMETER) {
            return;
        }
        CloseHandle(hToken);
    }
}

void JVMLauncher::LaunchJVM() {
    jvmDll = getDLLFromRegistry();
    javaHome = getJavaHomeFromRegistry();
    disableFolderVirtualisation();
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
    JavaVMOption options[jvmargs.size() + 3];
    options[0].optionString = (char*) (strJavaClassPath.c_str());
    options[1].optionString = (char*) (strJavaLibraryPath.c_str());
    options[2].optionString = (char*) "exit";
    options[2].extraInfo = (void*) *JVMLauncher::exit;
    for (int i = 3; i < jvmargs.size(); i++) {
        options[i].optionString = (char*) appargs[i].c_str();
    }
    //Configure VM args
    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.options = options;
    vm_args.nOptions = 3;
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
    //Get main class
    mainClass = jvmEnv->FindClass((char*) mainClassName.c_str());
    checkForException();
    //Get main method
    mainMethod = jvmEnv->GetStaticMethodID(mainClass, "main", "([Ljava/lang/String;)V");
    checkForException();
    //Attach to main thread
    jvm->AttachCurrentThread((LPVOID*) & jvmEnv, NULL);
    //Get main method args
    jclass StringClass = jvmEnv->FindClass("java/lang/String");
    jobjectArray jargs = jvmEnv->NewObjectArray(appargs.size(), StringClass, jvmEnv->NewStringUTF(""));
    for (int i = 0; i < appargs.size(); i++) {
        jvmEnv->SetObjectArrayElement(jargs, i, jvmEnv->NewStringUTF((char*) appargs[i].c_str()));
    }
    //Call main method
    jvmEnv->CallStaticVoidMethod(mainClass, mainMethod, jargs);
    jvm->DetachCurrentThread();
    jvm->DestroyJavaVM();
}

void JVMLauncher::exit(jint status) {
    cout << "JVM quitting: " << status << endl;
}

void JVMLauncher::checkForException() {
    //Check exception happened
    jthrowable ex = jvmEnv->ExceptionOccurred();
    if (ex != NULL) {
        //clear exception
        jvmEnv->ExceptionClear();
        //Grab info about exception and throw
        jmethodID toString = jvmEnv->GetMethodID(jvmEnv->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;");
        jstring estring = (jstring) jvmEnv->CallObjectMethod(ex, toString);
        jboolean isCopy;
        std::string message = jvmEnv->GetStringUTFChars(estring, &isCopy);
        throw JVMLauncherException(message);
    }
}
