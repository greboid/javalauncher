#include "JVMLauncherUtils.h"

void JVMLauncherUtils::checkForException(JNIEnv* env) {
	if (env->ExceptionCheck()) {
		jthrowable ex = env->ExceptionOccurred();
		if (ex != NULL) {
			env->ExceptionClear();
			jboolean isCopy;
			std::string message = env->GetStringUTFChars((jstring)env->CallObjectMethod(ex, env->GetMethodID(env->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;")), &isCopy);
			throw JVMLauncherException(message);
		}
	}
}

void JVMLauncherUtils::registerNativeMethod(JNIEnv* env, jclass clazz, std::string methodName, std::string methodSignature, void* pointer) {
    const JNINativeMethod methods[] = {
        { (char*) methodName.c_str(), (char*) methodSignature.c_str(), pointer }
    };
    const int methods_size = sizeof(methods) / sizeof(methods[0]);
    env->RegisterNatives(clazz, methods, methods_size);
    checkForException(env);
}

jclass JVMLauncherUtils::getClass(JNIEnv* env, std::string className) {
	if (className.empty()) {
		throw JVMLauncherException("Class cannot be undefined.");
	}
    jclass clazz = env->FindClass((char*) className.c_str());
    checkForException(env);
	if (clazz == NULL) {
		throw JVMLauncherException("Unable to find class: " + className);
	}
    return clazz;
}

jmethodID JVMLauncherUtils::getMethod(JNIEnv* env, jclass clazz, std::string methodName, std::string methodSignature) {
	if (methodName.empty()) {
		throw JVMLauncherException("Method name cannot be undefined.");
	}
	if (methodSignature.empty()) {
		throw JVMLauncherException("Method signature cannot be undefined.");
	}
    jmethodID method = env->GetStaticMethodID(clazz, (char*) methodName.c_str(), (char*) methodSignature.c_str());
    checkForException(env);
	if (method == NULL) {
		throw JVMLauncherException("Unable to find method in class: " + methodName);
	}
    return method;
}

void JVMLauncherUtils::callStaticVoidMethod(JNIEnv* env, std::string className, std::string methodName, std::string methodSignature, jobjectArray jargs) {
    JVMLauncherUtils::callStaticVoidMethod(env, JVMLauncherUtils::getClass(env, (char*) className.c_str()), methodName, methodSignature, jargs);
}

void JVMLauncherUtils::callStaticVoidMethod(JNIEnv* env, jclass clazz, std::string methodName, std::string methodSignature, jobjectArray jargs) {
    jmethodID method = JVMLauncherUtils::getMethod(env, clazz, (char*) methodName.c_str(), methodSignature);
    env->CallStaticVoidMethod(clazz, method, jargs);
	checkForException(env);
}


jobjectArray JVMLauncherUtils::convertCLIArgs(JNIEnv* env, std::vector<std::string> args) {
    jobjectArray jargs = env->NewObjectArray(args.size(), getClass(env, "java/lang/String"), env->NewStringUTF(""));
    for (unsigned int i = 0; i < args.size(); i++) {
        env->SetObjectArrayElement(jargs, i, env->NewStringUTF((char*) args[i].c_str()));
    }
    return jargs;
}
