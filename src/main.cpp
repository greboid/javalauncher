#include "log4z/log4z.h"
#include "jvmlauncher/JVMLauncher.h"
#include "singleinstance/SingleInstance.h"
#include "config/ConfigReader.h"
#include "config/ConfigDefaults.h"
#include "updater/Updater.h"
#include "utils/utils.h"
#include "version.h"
#include <sstream>
#include <Shlobj.h>
#include <windows.h>
#include <conio.h>

using namespace std;

int main(int argc, char** argv) {
	vector<string> cliArgs = Utils::arrayToVector(argc, argv);
	if (find(cliArgs.begin(), cliArgs.end(), "--LAUNCHER_VERSION") != cliArgs.end()) {
		LOGD("--LAUNCHER_VERSION");
		cout << LAUNCHER_VERSION << endl;
		exit(0);
	}
	if (find(cliArgs.begin(), cliArgs.end(), "--DEBUG") != cliArgs.end()) {
		cliArgs.erase(find(cliArgs.begin(), cliArgs.end(), "--DEBUG"));
		zsummer::log4z::ILog4zManager::GetInstance()->SetLoggerLevel(LOG4Z_MAIN_LOGGER_ID, 0);
	}
	zsummer::log4z::ILog4zManager::GetInstance()->Start();
	LOGD("Starting launcher.");
	LOGD("Checking update file.");
	std::ifstream file((char*)(Utils::GetAppDataDirectory() + Utils::getExeName()).c_str());
	std::string version = "-1";
	if (file.good()) {
		LOGD("Update file exists.");
		file.close();
		version = Utils::launchAppReturnOutput(Utils::GetAppDataDirectory() + Utils::getExeName());
	}
	LOGD("Disabling folder virtualisation.");
	Utils::disableFolderVirtualisation();
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
			std::string commandLine = Utils::getExeName() + " ";
			ShellExecute(NULL, LPSTR("open"), LPSTR((Utils::GetAppDataDirectory() + Utils::getExeName()).c_str()), LPSTR(argv), NULL, SW_SHOWNORMAL);
			exit(0);
		}
		LOGD("updater.doUpdate.");
		if (updater.doUpdate(directory)) {
			LOGD("Destroying JVM.");
			launcher->destroyJVM();
			LOGD("Relaunching.");
			updater.relaunch();
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
		_getch();
	}
	LOGD("Stopping single instance.");
	singleInstance.stopped();
	LOGD("Exiting.");
	return EXIT_SUCCESS;
}
