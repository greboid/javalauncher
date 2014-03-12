#ifndef JVMLAUNCHERUTILS_H
#define	JVMLAUNCHERUTILS_H

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

class JVMLauncherUtils {
public:
    static void checkForException(JNIEnv*);
    static jobjectArray convertCLIArgs(JNIEnv*, std::vector<std::string>);
    static void callStaticVoidMethod(JNIEnv*, std::string, std::string, std::string, jobjectArray);
    static void callStaticVoidMethod(JNIEnv*, jclass, std::string, std::string, jobjectArray);
    static void registerNativeMethod(JNIEnv*, jclass clazz, std::string methodName, std::string methodSignature, void* pointer);
    static jclass getClass(JNIEnv*, std::string);
    static jmethodID getMethod(JNIEnv*, jclass, std::string, std::string);
private:
protected:
};

#endif	/* JVMLAUNCHERUTILS_H */

