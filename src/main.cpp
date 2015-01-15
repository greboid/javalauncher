#include "log4z.h"
#include "JVMLauncher.h"
#include "SingleInstance.h"
#include "ConfigReader.h"
#include "ConfigDefaults.h"
#include "Updater.h"
#include "utils.h"
#include "platform.h"
#include <sstream>

using namespace std;

int main(int argc, char** argv) {
	zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerLevel(LOG4Z_MAIN_LOGGER_ID, 0);
	zsummer::log4z::ILog4zManager::GetInstance()->Start();
	vector<string> cliArgs = Utils::arrayToVector(argc, argv);
	if (find(cliArgs.begin(), cliArgs.end(), "--LAUNCHER_VERSION") != cliArgs.end()) {
		cout << LAUNCHER_VERSION << endl;
		exit(0);
	}
	if (find(cliArgs.begin(), cliArgs.end(), "--DEBUG") != cliArgs.end()) {
		cliArgs.erase(find(cliArgs.begin(), cliArgs.end(), "--DEBUG"));
		Platform::createConsole();
	}
	LOGD("Starting launcher.");
	LOGD("Checking update file.");
	std::ifstream file((char*)(Platform::GetAppDataDirectory() + Utils::getExeName()).c_str());
	LOGD("File: ");
	std::string version = "-1";
	if (file.good()) {
		LOGD("Update file exists.");
		file.close();
		version = Utils::launchAppReturnOutput(Platform::GetAppDataDirectory() + Utils::getExeName(), argv);
	}
	LOGD("Creating config.");
	ConfigReader config;
	LOGD("Creating single instance.");
	SingleInstance singleInstance(config);
	if (!singleInstance.getCanStart()) {
		LOGE("Another instance already running.");
		return EXIT_FAILURE;
	}
	LOGD("Creating updater.");
	Updater updater(config);
	try {
		LOGD("Moving application Updates.");
		updater.moveApplicationUpdates();
		LOGD("Creating JVMLauncher.");
		JVMLauncher* launcher = new JVMLauncher(cliArgs, config);
		LOGD("Launching JVM");
		launcher->LaunchJVM();
		LOGD("Getting directory.");
		std::string directory = launcher->callGetDirectory();
		LOGD("Checking if existing is newer or older.");
		int compareValue = launcher->callIsNewer(version, LAUNCHER_VERSION);
		if (compareValue > 0) {
			LOGD("Version is newer, updating.");
			Platform::launchApplication((Platform::GetAppDataDirectory() + Utils::getExeName()), Utils::vectorToString(Utils::arrayToVector(argc, argv)));
			exit(0);
		}
		LOGD("updater.doUpdate.");
		if (updater.doUpdate(directory)) {
			LOGD("Destroying JVM.");
			launcher->destroyJVM();
			LOGD("Relaunching.");
			updater.relaunch(Utils::vectorToString(Utils::arrayToVector(argc, argv)));
		}
		LOGD("Calling main method.");
		launcher->callMainMethod();
		LOGD("Destroying JVM.");
		launcher->destroyJVM();
	}
	catch (JVMLauncherException& ex) {
		LOGE("Launching the JVM failed: ");
		LOGE(ex.what());
		LOGE("Press any key to exit");
	}
	LOGD("Stopping single instance.");
	//singleInstance.stopped();
	LOGD("Exiting.");
	return EXIT_SUCCESS;
}
