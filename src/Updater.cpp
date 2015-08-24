#include "Updater.h"

using namespace std;

Updater::Updater(ConfigReader& config) {
	this->config = config;
	this->newVersion = "";
}

void Updater::relaunch(std::string args) {
	BOOST_LOG_TRIVIAL(debug) << "Creating new process.";
	Platform::launchApplication(Utils::getExePathAndName(), args);
	BOOST_LOG_TRIVIAL(debug) << "Releasing mutex";
	updateMutex.unlock();
	BOOST_LOG_TRIVIAL(debug) << "Exiting app.";
	exit(0);
}

void Updater::getAndLockMutex() {
	BOOST_LOG_TRIVIAL(debug) << "Creating mutex.";
	updateMutex = Mutex();
	updateMutex.init("DMDirc-Updater");
	BOOST_LOG_TRIVIAL(debug) << "Waiting for mutex.";
	updateMutex.lock();
}

void Updater::deleteOldLauncher() {
	BOOST_LOG_TRIVIAL(debug) << "Checking for old launcher.";
	Platform::deleteFileIfExists(Utils::getExePathAndName() + ".old");
}

bool Updater::doUpdate(std::string directory) {
	getAndLockMutex();
	deleteOldLauncher();
	bool relaunchNeeded = false;
	BOOST_LOG_TRIVIAL(debug) << "Checking launcher auto update = " << LAUNCHER_AUTOUPDATE;
	if (config.getBoolValue("launcher.autoupdate", LAUNCHER_AUTOUPDATE)) {
		BOOST_LOG_TRIVIAL(debug) << "Attempting to update launcher.";
		int success = updateLauncher(directory, Utils::getExePath());
		BOOST_LOG_TRIVIAL(debug) << "Update result: " << success;
		if (success == -1) {
			BOOST_LOG_TRIVIAL(debug) << "Failed: attempting to update app data";
			success = updateLauncher(directory, Platform::GetAppDataDirectory());
			BOOST_LOG_TRIVIAL(debug) << "Update result: " << success;
		}
		if (success == 1) {
			BOOST_LOG_TRIVIAL(debug) << "Success, restart needed.";
			relaunchNeeded = true;
		}
	}
	BOOST_LOG_TRIVIAL(debug) << "Checking application update = " << APPLICATION_AUTOUPDATE;
	if (config.getBoolValue("application.autoupdate", APPLICATION_AUTOUPDATE)) {
		BOOST_LOG_TRIVIAL(debug) << "Attempting to update application.";
		int success = updateApplication(directory, Utils::getExePath());
		if (success == -1) {
			BOOST_LOG_TRIVIAL(debug) << "Failed: Updating to app data.";
			updateApplication(directory, Platform::GetAppDataDirectory());
		}
		if (success == 1) {
			relaunchNeeded = true;
		}
	}
	BOOST_LOG_TRIVIAL(debug) << "Releasing mutex.";
	updateMutex.unlock();
	BOOST_LOG_TRIVIAL(debug) << "Returning value: " << relaunchNeeded;
	return relaunchNeeded;
}

int Updater::updateLauncher(std::string from, std::string to) {
	std::ifstream file((char*)(from + "/" + Utils::getExeName()).c_str());
	if (file.good()) {
		BOOST_LOG_TRIVIAL(debug) << "Update file exists";
		file.close();
		BOOST_LOG_TRIVIAL(debug) << "Attempting to backup existing launcher.";
		if (Platform::moveFile(Utils::getExePathAndName(), Utils::getExePathAndName() + ".old")) {
			BOOST_LOG_TRIVIAL(debug) << "Unable to backup existing launcher.";
			return -1;
		}
		if (!Platform::moveFile(from + "/" + Utils::getExeName(), to + Utils::getExePath())) {
			BOOST_LOG_TRIVIAL(debug) << "Moving new launcher failed.";
			return -1;
		}
		BOOST_LOG_TRIVIAL(debug) << "Updating launcher suceeded.";
		return 1;
	} else {
		file.close();
		BOOST_LOG_TRIVIAL(debug) << "Updating launcher not required.";
		return 0;
	}
}

void Updater::moveApplicationUpdates() {
	std::string to = Utils::getExePath();
	vector<string> files = Utils::addMatchingFilesToVector(to, std::regex(".*\.tmp"));
	for (unsigned int i = 0; i < files.size(); i++) {
		std::string updateSource = files[i];
		std::string updateTarget = files[i].substr(0, files[i].length() - 4);
		if (Platform::moveFile(to + updateSource, to + updateTarget)) {
			BOOST_LOG_TRIVIAL(debug) << "Updating suceeded.";
		}
		else {
			BOOST_LOG_TRIVIAL(debug) << "Updating failed.";
		}
	}
}

int Updater::updateApplication(std::string from, std::string to) {
	vector<string> files = Utils::addMatchingFilesToVector(to, std::regex(".*"));
	if (files.size() == 0) {
		BOOST_LOG_TRIVIAL(debug) << "Updating application not required.";
		return 0;
	}
	else {
		BOOST_LOG_TRIVIAL(debug) << "Updating application: " << files.size();
	}
	int restartNeeded = 0;
	for (unsigned int i = 0; i < files.size(); i++) {
		BOOST_LOG_TRIVIAL(debug) << "Attempting to update: " << files[i];
		std::string updateSource = "." + files[i];
		std::string updateTarget = files[i];
		std::ifstream file((char*)(from + "/" + updateSource).c_str());
		if (file.good()) {
			file.close();
			BOOST_LOG_TRIVIAL(debug) << "Update file exists, trying to copy.";
			BOOST_LOG_TRIVIAL(debug) << "Updating: " << from + updateSource << " => " << to + updateTarget;
			if (Platform::moveFile(from + updateSource, to + updateTarget + ".tmp")) {
				BOOST_LOG_TRIVIAL(debug) << "Updating suceeded.";
				restartNeeded = 1;
			}
			else {
				BOOST_LOG_TRIVIAL(debug) << "Updating failed.";
				restartNeeded = 0;
			}
		}
		else {
			BOOST_LOG_TRIVIAL(debug) << "Update file does not exist.";
		}
		
	}
	return restartNeeded;
}