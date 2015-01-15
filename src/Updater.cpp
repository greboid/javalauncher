#include "Updater.h"

using namespace std;

Updater::Updater(ConfigReader& config) {
	this->config = config;
	this->newVersion = "";
}

void Updater::relaunch(std::string args) {
	LOGD("Creating new process.");
	Platform::launchApplication(Utils::getExePathAndName(), args);
	LOGD("Releasing mutex");
	updateMutex.unlock();
	LOGD("Exiting app.");
	exit(0);
}

void Updater::getAndLockMutex() {
	LOGD("Creating mutex.");
	updateMutex = Mutex();
	updateMutex.init("DMDirc-Updater");
	LOGD("Waiting for mutex.");
	updateMutex.lock();
}

void Updater::deleteOldLauncher() {
	LOGD("Checking for old launcher.");
	Platform::deleteFileIfExists(Utils::getExePathAndName() + ".old");
}

bool Updater::doUpdate(std::string directory) {
	getAndLockMutex();
	deleteOldLauncher();
	bool relaunchNeeded = false;
	LOGD("Checking launcher auto update = " << LAUNCHER_AUTOUPDATE);
	if (config.getBoolValue("launcher.autoupdate", LAUNCHER_AUTOUPDATE)) {
		LOGD("Attempting to update launcher.");
		int success = updateLauncher(directory, Utils::getExePath());
		LOGD("Update result: " << success);
		if (success == -1) {
			LOGD("Failed: attempting to update app data");
			success = updateLauncher(directory, Platform::GetAppDataDirectory());
			LOGD("Update result: " << success);
		}
		if (success == 1) {
			LOGD("Success, restart needed.");
			relaunchNeeded = true;
		}
	}
	LOGD("Checking application update = " << APPLICATION_AUTOUPDATE);
	if (config.getBoolValue("application.autoupdate", APPLICATION_AUTOUPDATE)) {
		LOGD("Attempting to update application.");
		int success = updateApplication(directory, Utils::getExePath());
		if (success == -1) {
			LOGD("Failed: Updating to app data.");
			updateApplication(directory, Platform::GetAppDataDirectory());
		}
		if (success == 1) {
			relaunchNeeded = true;
		}
	}
	LOGD("Releasing mutex.");
	updateMutex.unlock();
	LOGD("Returning value: " << relaunchNeeded);
	return relaunchNeeded;
}

int Updater::updateLauncher(std::string from, std::string to) {
	std::ifstream file((char*)(from + "/" + Utils::getExeName()).c_str());
	if (file.good()) {
		LOGD("Update file exists");
		file.close();
		LOGD("Attempting to backup existing launcher.");
		if (Platform::moveFile(Utils::getExePathAndName(), Utils::getExePathAndName() + ".old")) {
			LOGD("Unable to backup existing launcher.");
			return -1;
		}
		if (!Platform::moveFile(from + "/" + Utils::getExeName(), to + Utils::getExePath())) {
			LOGD("Moving new launcher failed.");
			return -1;
		}
		LOGD("Updating launcher suceeded.");
		return 1;
	} else {
		file.close();
		LOGD("Updating launcher not required.");
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
			LOGD("Updating suceeded.");
		}
		else {
			LOGD("Updating failed.");
		}
	}
}

int Updater::updateApplication(std::string from, std::string to) {
	vector<string> files = Utils::addMatchingFilesToVector(to, std::regex(".*"));
	if (files.size() == 0) {
		LOGD("Updating application not required.");
		return 0;
	}
	else {
		LOGD("Updating application: " << files.size());
	}
	int restartNeeded = 0;
	for (unsigned int i = 0; i < files.size(); i++) {
		LOGD("Attempting to update: " << files[i]);
		std::string updateSource = "." + files[i];
		std::string updateTarget = files[i];
		std::ifstream file((char*)(from + "/" + updateSource).c_str());
		if (file.good()) {
			file.close();
			LOGD("Update file exists, trying to copy.");
			LOGD("Updating: " << from + updateSource << " => " << to + updateTarget);
			if (Platform::moveFile(from + updateSource, to + updateTarget + ".tmp")) {
				LOGD("Updating suceeded.");
				restartNeeded = 1;
			}
			else {
				LOGD("Updating failed.");
				restartNeeded = 0;
			}
		}
		else {
			LOGD("Update file does not exist.");
		}
		
	}
	return restartNeeded;
}