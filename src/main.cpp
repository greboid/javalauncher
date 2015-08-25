#include "JVMLauncher.h"
#include "SingleInstance.h"
#include "ConfigDefaults.h"
#include "Updater.h"
#include "utils.h"
#include "platform.h"
#include "gitversion.h"
#include "Logger.h"
#include <sstream>
#include <boost/program_options.hpp>
#include <iostream>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv) {
	po::options_description cli("Command Line Options");
	cli.add_options()
		("help,h", "Print help messages")
		("debug,d", "Show debug information")
		("version,v", "Shows the launcher version")
		;
	po::options_description conf("Configuration Options");
	conf.add_options()
		("jvm.args", po::value<string>()->default_value(""), "Arguments to pass to the JVM in addition to -Dfile.encoding=utf-8")
		("application.name",po::value<string>()->default_value(APPLICATION_NAME), "The application's name")
		("application.main", po::value<string>()->default_value(APPLICATION_MAIN), "The application's Main class")
		("application.args", po::value<string>()->default_value(""), "Arguments to pass to the application")
		("application.path", po::value<string>()->default_value(APPLICATION_PATH), "The path of the application")
		("application.autoupdate", po::value<bool>()->default_value(APPLICATION_AUTOUPDATE), "Should the launcher auto update the application")
		("launcher.main", po::value<string>()->default_value(APPLICATION_SETUP), "Launcher Utilities class  (See docs for API)")
		("launcher.singleinstance", po::value<bool>()->default_value(LAUNCHER_SINGLEINSTANCE), "Should we use a global single instance")
		("launcher.autoupdate", po::value<bool>()->default_value(LAUNCHER_AUTOUPDATE), "Should the launcher auto update itself")
		;
	po::variables_map options;
	try {
		po::store(po::parse_command_line(argc, argv, cli), options);
	} catch (po::error& e) {
		BOOST_LOG_TRIVIAL(error) << "ERROR: " << e.what() << std::endl << std::endl;
		return 1;
	}
	
	try {
		ifstream file((char*)(Utils::getExePath() + "//" + "launcher.config").c_str(), ios::in);
		po::store(po::parse_config_file(file, conf), options);
		if (options.count("help")) {
			std::cout << cli << std::endl;
			std::cout << conf << std::endl;
			return 0;
		}
		po::notify(options);
	} catch (po::error& e) {
		cerr << "ERROR: " << e.what() << std::endl << std::endl;
		return 1;
	}
	Logger::init(options.count("debug"), APPLICATION_NAME);
	if (options.count("debug") != 1) {
		FreeConsole();
	}
	vector<string> cliArgs = Utils::arrayToVector(argc, argv);
	if (find(cliArgs.begin(), cliArgs.end(), "--LAUNCHER_VERSION") != cliArgs.end()) {
		cout << LAUNCHER_VERSION << endl;
		exit(0);
	}
	if (find(cliArgs.begin(), cliArgs.end(), "--DEBUG") != cliArgs.end()) {
		cliArgs.erase(find(cliArgs.begin(), cliArgs.end(), "--DEBUG"));
		Platform::createConsole();
	}
	BOOST_LOG_TRIVIAL(debug) << "Starting launcher.";
	BOOST_LOG_TRIVIAL(debug) << "Checking update file.";
	std::ifstream file((char*)(Platform::GetAppDataDirectory() + Utils::getExeName()).c_str());
	std::string version = "-1";
	if (file.good()) {
		BOOST_LOG_TRIVIAL(debug) << "Update file exists.";
		file.close();
		version = Utils::launchAppReturnOutput(Platform::GetAppDataDirectory() + Utils::getExeName(), argv);
	}
	BOOST_LOG_TRIVIAL(debug) << "Creating single instance.";
	SingleInstance singleInstance(options);
	if (!singleInstance.getCanStart()) {
		BOOST_LOG_TRIVIAL(error) << "Another instance already running.";
		return EXIT_FAILURE;
	}
	BOOST_LOG_TRIVIAL(debug) << "Creating updater.";
	Updater updater(options);
	try {
		BOOST_LOG_TRIVIAL(debug) << "Moving application Updates.";
		updater.moveApplicationUpdates();
		BOOST_LOG_TRIVIAL(debug) << "Creating JVMLauncher.";
		JVMLauncher* launcher = new JVMLauncher(cliArgs, options);
		BOOST_LOG_TRIVIAL(debug) << "Launching JVM";
		launcher->LaunchJVM();
		BOOST_LOG_TRIVIAL(debug) << "Getting directory.";
		std::string directory = launcher->callGetDirectory();
		BOOST_LOG_TRIVIAL(debug) << "Checking if existing is newer or older.";
		int compareValue = launcher->callIsNewer(version, LAUNCHER_VERSION);
		if (compareValue > 0) {
			BOOST_LOG_TRIVIAL(debug) << "Version is newer, updating.";
			Platform::launchApplication((Platform::GetAppDataDirectory() + Utils::getExeName()), Utils::vectorToString(Utils::arrayToVector(argc, argv)));
			exit(0);
		}
		BOOST_LOG_TRIVIAL(debug) << "updater.doUpdate.";
		if (updater.doUpdate(directory)) {
			BOOST_LOG_TRIVIAL(debug) << "Destroying JVM.";
			launcher->destroyJVM();
			BOOST_LOG_TRIVIAL(debug) << "Relaunching.";
			updater.relaunch(Utils::vectorToString(Utils::arrayToVector(argc, argv)));
		}
		BOOST_LOG_TRIVIAL(debug) << "Calling main method.";
		launcher->callMainMethod();
		BOOST_LOG_TRIVIAL(debug) << "Destroying JVM.";
		launcher->destroyJVM();
	}
	catch (JVMLauncherException& ex) {
		BOOST_LOG_TRIVIAL(error) << "Launching the JVM failed: ";
		BOOST_LOG_TRIVIAL(error) << ex.what();
		BOOST_LOG_TRIVIAL(error) << "Press any key to exit";
	}
	BOOST_LOG_TRIVIAL(debug) << "Stopping single instance.";
	//singleInstance.stopped();
	BOOST_LOG_TRIVIAL(debug) << "Exiting.";
	return EXIT_SUCCESS;
}
