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
	zsummer::log4z::ILog4zManager::GetInstance()->Start();
	LOGD("Starting Launcher.");
	if (argc >= 2 && argv[1] == std::string("--LAUNCHER_VERSION")) {
		LOGD("--LAUNCHER_VERSION");
		cout << LAUNCHER_VERSION << endl;
		exit(0);
	}
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
		cout << "Another instance already running." << endl;
		return EXIT_FAILURE;
	}
	LOGD("Creating updater.");
	Updater updater(config);
	try {
		LOGD("Creating JVMLauncher.");
		JVMLauncher* launcher = new JVMLauncher(Utils::arrayToVector(argc, argv), config);
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
		LOGD("Updating.");
		if (updater.doUpdate(directory)) {
			launcher->destroyJVM();
			updater.relaunch();
		}
		LOGD("Calling main method.");
		launcher->callMainMethod();
		LOGD("Destroying JVM.");
		launcher->destroyJVM();
	}
	catch (JVMLauncherException& ex) {
		cout << "Launching the JVM failed: ";
		cout << ex.what() << endl;
		cout << "Press any key to exit" << endl;
		_getch();
	}
	cout << "Tidying up and exiting." << endl;
	singleInstance.stopped();
	return EXIT_SUCCESS;
}
