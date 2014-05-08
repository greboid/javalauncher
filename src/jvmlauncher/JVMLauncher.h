#ifndef JVMLAUNCHER_H
#define	JVMLAUNCHER_H

#include "log4z/log4z.h"
#include "version.h"
#include "config/ConfigDefaults.h"
#include "config/ConfigReader.h"
#include "updater/Updater.h"
#include "JVMLauncherUtils.h"
#include <stdexcept>
#include <jni.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include <process.h>

class JVMLauncher {
public:
    JVMLauncher(std::vector<std::string>, ConfigReader&);
    void LaunchJVM();
    void destroyJVM();
	std::string callGetDirectory();
    void callMainMethod();
	int callIsNewer(std::string, std::string);
private:
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
	vector<string> getCliArgs(vector<string>, ConfigReader& config);
	vector<string> getJvmArgs(ConfigReader config);
    std::string getDLLFromRegistry();
    std::string getRegistryValue(std::string, std::string);
    std::string getJavaHomeFromRegistry();
    void addAllJarsFromPath(std::string);
protected:
};

#endif	/* JVMLAUNCHER_H */

