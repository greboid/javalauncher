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
#include "../config/ConfigReader.h"
#include "JVMLauncherUtils.h"

class JVMLauncher {
public:
    JVMLauncher(std::vector<std::string>, std::vector<std::string>, ConfigReader&);
    void LaunchJVM();
    void destroyJVM();
	std::string callGetDirectory();
    void callMainMethod();
	int callIsNewer(std::string, std::string);
    HANDLE forkAndLaunch();
private:
    static UINT WINAPI threadEntry(LPVOID);
    static void exit(jint);
    typedef jint(JNICALL* CreateJavaVM)(JavaVM**, void**, void*);
    ConfigReader config;
    HINSTANCE jvmDllInstance;
    std::vector<std::string> jvmargs;
    std::vector<std::string> appargs;
    std::string mainClassName;
	std::string utilsClassName;
    std::string javaHome;
    std::string appHome;
    std::string jvmDll;
    std::vector<std::string> jars;
    CreateJavaVM jvmInstance;
    jclass mainClass;
    jmethodID mainMethod;
    JNIEnv* jvmEnv;
    JavaVM* jvm;
    std::string getDLLFromRegistry();
    std::string getRegistryValue(std::string, std::string);
    std::string getJavaHomeFromRegistry();
    void addAllJarsFromPath(std::string);
protected:
};

#endif	/* JVMLAUNCHER_H */

