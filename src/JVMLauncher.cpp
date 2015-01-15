#include "JVMLauncher.h"

using namespace std;

vector<string> JVMLauncher::getCliArgs(vector<std::string> cliArgs, ConfigReader& config) {
	cliArgs.erase(cliArgs.begin());
	cliArgs.push_back("-l");
	cliArgs.push_back(std::string("bob-") + std::string(LAUNCHER_VERSION));
	vector<string> newCliArgs = Utils::mergeVectors(config.getVectorValue("application.args", vector<string>(0)), cliArgs);
	LOGD("CLI Args: " << newCliArgs.size());
	return newCliArgs;
}

vector<string> JVMLauncher::getJvmArgs(ConfigReader config) {
	vector<string> jvmArgs;
	jvmArgs.push_back("-Dfile.encoding=utf-8");
	jvmArgs = Utils::mergeVectors(config.getVectorValue("jvm.args", vector<string>(0)), jvmArgs);
	LOGD("JVM Args: " << jvmArgs.size());
	return jvmArgs;
}

JVMLauncher::JVMLauncher(vector<std::string> appargs, ConfigReader& config) {
	std::string path = config.getStringValue("application.path", APPLICATION_PATH);
	//set application home
	appHome.append(path);
	//add all jars from path
	Utils::addMatchingFilesToExistingVector(jars, path, std::regex(".*\\.jar"));
	if (jars.size() == 0) {
		throw JVMLauncherException("No jar files found.");
	}
	this->mainClassName = config.getStringValue("application.main", APPLICATION_MAIN);
	this->utilsClassName = config.getStringValue("launcherutils.main", LAUNCHERUTILS_MAIN);
	this->config = config;
	this->jvmargs = getJvmArgs(config);
	this->appargs = getCliArgs(appargs, config);
	this->jvmEnv = NULL;
	this->jvm = NULL;
}

void JVMLauncher::LaunchJVM() {
	LOGD("Getting DLL from registry");
	std::string jvmDll = Platform::getJavaDLLFromRegistry();
	LOGD("Setting Java Home from registry")
	javaHome = Platform::getJavaHomeFromRegistry();
	//Build library path
	std::string strJavaLibraryPath = "-Djava.library.path=";
	strJavaLibraryPath += javaHome + "\\lib" + "," + javaHome + "\\jre\\lib";
	LOGD("Java Library Path: " << strJavaLibraryPath);
	//Add jars to classpath
	std::string strJavaClassPath = "-Djava.class.path=";
	if (jars.size() > 0) {
		for (unsigned int i = 0; i < jars.size() - 1; i++) {
			strJavaClassPath += appHome + jars[i] + ";";
		}
		strJavaClassPath += appHome + jars[jars.size() - 1];
	}
	LOGD("Java Class Path: " << strJavaClassPath);
	//Configure JVM Options
	JavaVMOption options[100];
	options[0].optionString = (char*)(strJavaClassPath.c_str());
	options[1].optionString = (char*)(strJavaLibraryPath.c_str());
	options[2].optionString = (char*) "exit";
	options[2].extraInfo = (void*)*JVMLauncher::exit;
	for (unsigned int i = 3; i < jvmargs.size() + 3; i++) {
		options[i].optionString = (char*)jvmargs[(i - 3)].c_str();
	}
	//Configure VM args
	JavaVMInitArgs vm_args;
	vm_args.version = JNI_VERSION_1_6;
	vm_args.options = options;
	vm_args.nOptions = jvmargs.size() + 3;
	vm_args.ignoreUnrecognized = JNI_FALSE;
	//Load JVM.dll
	jvmInstance = Platform::getJVMInstance(jvmDll);
	//Create the JVM
	jint res = jvmInstance(&jvm, (void **)&jvmEnv, &vm_args);
	if (res < 0) {
		throw JVMLauncherException("Could not launch the JVM");
	}
	//Attach to main thread
	jvm->AttachCurrentThread((void **)&jvmEnv, NULL);
}

void JVMLauncher::destroyJVM() {
	jvm->DetachCurrentThread();
	jvm->DestroyJavaVM();
}

void JVMLauncher::callMainMethod() {
	jobjectArray jargs = JVMLauncherUtils::convertCLIArgs(jvmEnv, appargs);
	JVMLauncherUtils::callStaticVoidMethod(jvmEnv, mainClassName, std::string("main"), std::string("([Ljava/lang/String;)V"), jargs);
}

std::string JVMLauncher::callGetDirectory() {
	jobjectArray jargs = JVMLauncherUtils::convertCLIArgs(jvmEnv, appargs);
	std::string clazzName(utilsClassName);
	jclass clazz = JVMLauncherUtils::getClass(jvmEnv, clazzName);
	jmethodID method = JVMLauncherUtils::getMethod(jvmEnv, clazz, "getDirectory", "([Ljava/lang/String;)Ljava/lang/String;");
	jstring moo = (jstring) jvmEnv->CallStaticObjectMethod(clazz, method, jargs);
	const char* string = jvmEnv->GetStringUTFChars(moo, NULL);
	std::string directory = std::string(string);
	jvmEnv->ReleaseStringUTFChars(moo, string);
	return directory;
}

int JVMLauncher::callIsNewer(std::string version1, std::string version2) {
	std::string clazzName(utilsClassName);
	jclass clazz = JVMLauncherUtils::getClass(jvmEnv, clazzName);
	jmethodID method = JVMLauncherUtils::getMethod(jvmEnv, clazz, "getIsNewer", "(Ljava/lang/String;Ljava/lang/String;)I");
	jint moo = jvmEnv->CallStaticIntMethod(clazz, method, jvmEnv->NewStringUTF((char*)version1.c_str()), jvmEnv->NewStringUTF((char*)version2.c_str()));
	JVMLauncherUtils::checkForException(jvmEnv);
	return moo;
}

void JVMLauncher::exit(jint status) {
	LOGD("JVM quitting: " << status);
}
