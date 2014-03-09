#ifndef JVMLAUNCHER_H
#define	JVMLAUNCHER_H

#include <stdexcept>
#include <windows.h>
#include <tchar.h>
#include <jni.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include <process.h>
#include <windows.h>
#include <dirent.h>
#include "JVMLauncherException.cpp"
#include "../config/ConfigReader.h"

class JVMLauncher {
public:
    JVMLauncher(std::string, std::string, std::vector<std::string>, std::vector<std::string>, ConfigReader*);
    void LaunchJVM();
    HANDLE forkAndLaunch();
private:
    static UINT WINAPI threadEntry(LPVOID);
    static void exit(jint);
    typedef jint(JNICALL *CreateJavaVM)(JavaVM**, void**, void*);
    ConfigReader* config;
    HINSTANCE jvmDllInstance;
    std::vector<std::string> jvmargs;
    std::vector<std::string> appargs;
    std::string mainClassName;
    std::string javaHome;
    std::string appHome;
    std::string jvmDll;
    std::vector<std::string> jars;
    CreateJavaVM jvmInstance;
    jclass mainClass;
    jmethodID mainMethod;
    JNIEnv *jvmEnv;
    JavaVM *jvm;
    void checkForException();
    std::string getDLLFromRegistry();
    std::string getRegistryValue(std::string, std::string);
    std::string getJavaHomeFromRegistry();
    void disableFolderVirtualisation();
    void addAllJarsFromPath(std::string);
protected:
};

#endif	/* JVMLAUNCHER_H */

