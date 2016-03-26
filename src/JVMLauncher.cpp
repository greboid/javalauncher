#include "JVMLauncher.h"

using namespace std;

vector<string> JVMLauncher::getCliArgs(vector<std::string> cliArgs, boost::program_options::variables_map& config) {
	cliArgs.erase(cliArgs.begin());
	cliArgs.push_back("-l");
	cliArgs.push_back(std::string("bob-") + std::string(LAUNCHER_VERSION));
	vector<string> newCliArgs;
	if ((config["application.args"].as<string>()).compare("") != 0) {
		newCliArgs = Utils::mergeVectors(config["application.args"].as<vector<string>>(), cliArgs);
	}
	BOOST_LOG_TRIVIAL(debug) << "CLI Args: " << newCliArgs.size();
	return newCliArgs;
}

vector<string> JVMLauncher::getJvmArgs(boost::program_options::variables_map config) {
	vector<string> jvmArgs;
	BOOST_LOG_TRIVIAL(debug) << "Adding UTF-8.";
	jvmArgs.push_back("-Dfile.encoding=utf-8");
	BOOST_LOG_TRIVIAL(debug) << "Merging config items in.";
	if ((config["jvm.args"].as<string>()).compare("") != 0) {
		BOOST_LOG_TRIVIAL(debug) << "Config JVM Args: " << config["jvm.args"].as<string>() << ".";
		jvmArgs = Utils::mergeVectors(config["jvm.args"].as<vector<string>>(), jvmArgs);
	}
	BOOST_LOG_TRIVIAL(debug) << "JVM Args: " << jvmArgs.size();
	return jvmArgs;
}

JVMLauncher::JVMLauncher(vector<std::string> appargs, boost::program_options::variables_map& config) {
	std::string path = config["application.path"].as<string>();
	//set application home
	appHome.append(path);
	BOOST_LOG_TRIVIAL(debug) << "Finding all jars";
	Utils::addMatchingFilesToExistingVector(jars, path, std::regex(".*\\.jar"));
	if (jars.size() == 0) {
		throw JVMLauncherException("No jar files found.");
	}
	BOOST_LOG_TRIVIAL(debug) << "Setting up JNI.";
	this->mainClassName = config["application.main"].as<string>();
	this->utilsClassName = config["launcher.main"].as<string>();
	this->config = config;
	BOOST_LOG_TRIVIAL(debug) << "Getting JVM Arguments.";
	this->jvmargs = getJvmArgs(config);
	BOOST_LOG_TRIVIAL(debug) << "Getting CLI arguments.";
	this->appargs = getCliArgs(appargs, config);
	this->jvmEnv = NULL;
	this->jvm = NULL;
	BOOST_LOG_TRIVIAL(debug) << "Finished setting up JNI.";
}

void JVMLauncher::launchJVM() {
	BOOST_LOG_TRIVIAL(debug) << "Getting DLL from registry";
	std::string jvmDll = Platform::getJavaDLLFromRegistry();
	BOOST_LOG_TRIVIAL(debug) << "Setting Java Home from registry";
	javaHome = Platform::getJavaHomeFromRegistry();
	//Build library path
	std::string strJavaLibraryPath = "-Djava.library.path=";
	strJavaLibraryPath += javaHome + "\\lib" + "," + javaHome + "\\jre\\lib";
	BOOST_LOG_TRIVIAL(debug) << "Java Library Path: " << strJavaLibraryPath;
	//Add jars to classpath
	std::string strJavaClassPath = "-Djava.class.path=";
	if (jars.size() > 0) {
		for (unsigned int i = 0; i < jars.size() - 1; i++) {
			strJavaClassPath += appHome + jars[i] + ";";
		}
		strJavaClassPath += appHome + jars[jars.size() - 1];
	}
	BOOST_LOG_TRIVIAL(debug) << "Java Class Path: " << strJavaClassPath;
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
	BOOST_LOG_TRIVIAL(debug) << "JVM quitting: " << status;
}
