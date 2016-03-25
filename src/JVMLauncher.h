#ifndef JVMLAUNCHER_H
#define	JVMLAUNCHER_H

#include "Logger.h"
#include "ConfigDefaults.h"
#include "Updater.h"
#include "JVMLauncherUtils.h"
#include "gitversion.h"
#include <stdexcept>
#include "jni.h"
#include <string>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>

class JVMLauncher {
public:
    JVMLauncher(std::vector<std::string>, boost::program_options::variables_map&);
    void launchJVM();
    void destroyJVM();
	std::string callGetDirectory();
    void callMainMethod();
	int callIsNewer(std::string, std::string);
private:
    static void exit(jint);
    typedef jint(JNICALL* CreateJavaVM)(JavaVM**, void**, void*);
	boost::program_options::variables_map config;
    std::vector<std::string> jvmargs;
    std::vector<std::string> appargs;
    std::string mainClassName;
	std::string utilsClassName;
    std::string javaHome;
    std::string appHome;
    std::vector<std::string> jars;
    CreateJavaVM jvmInstance;
    jclass mainClass;
    jmethodID mainMethod;
    JNIEnv* jvmEnv;
    JavaVM* jvm;
	vector<string> getCliArgs(vector<string>, boost::program_options::variables_map& config);
	vector<string> getJvmArgs(boost::program_options::variables_map config);
    void addAllJarsFromPath(std::string);
protected:
};

#endif	/* JVMLAUNCHER_H */

