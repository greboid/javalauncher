#include "JVMLauncher.h"

JVMLauncher::JVMLauncher(std::string path, std::string mainClassName, std::vector<std::string> jvmargs, std::vector<std::string> appargs, ConfigReader& config) {
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
    JavaVMOption options[jvmargs.size() + 3];
    options[0].optionString = (char*) (strJavaClassPath.c_str());
    options[1].optionString = (char*) (strJavaLibraryPath.c_str());
    options[2].optionString = (char*) "exit";
    options[2].extraInfo = (void*) *JVMLauncher::exit;
    for (int i = 3; i < jvmargs.size() + 3; i++) {
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
    jobjectArray jargs = JVMLauncherUtils::convertCLIArgs(jvmEnv, appargs);
    JVMLauncher::callLauncherUtils(jvmEnv, jargs);
    JVMLauncher::callMainMethod(jvmEnv, jargs);
    jvm->DetachCurrentThread();
    jvm->DestroyJavaVM();
}

void JVMLauncher::callMainMethod(JNIEnv* env, jobjectArray jargs) {
    JVMLauncherUtils::callStaticVoidMethod(jvmEnv, mainClassName, std::string("main"), std::string("([Ljava/lang/String;)V"), jargs);
}

void JVMLauncher::callLauncherUtils(JNIEnv* env, jobjectArray jargs) {
    std::string clazzName("com/dmdirc/LauncherUtils");
    jclass clazz = JVMLauncherUtils::getClass(env, clazzName);
    JVMLauncherUtils::registerNativeMethod(env, clazz, std::string("setDirectory"), std::string("(Ljava/lang/String;)V"), (void*)&JVMLauncher::setDirectory);
    JVMLauncherUtils::callStaticVoidMethod(env, clazzName, "getDirectory", "([Ljava/lang/String;)V", jargs);
}

void JVMLauncher::setDirectory(JNIEnv* env, jclass clazz, jstring string) {
    jboolean isCopy;
    std::string message = env->GetStringUTFChars(string, &isCopy);
    cout << "Config directory: " << message << endl;
}

void JVMLauncher::exit(jint status) {
    cout << "JVM quitting: " << status << endl;
}
