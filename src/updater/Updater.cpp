#include "Updater.h"

using namespace std;

Updater::Updater(ConfigReader& config) {
	this->config = config;
	this->newVersion = "";
}

bool Updater::doUpdate(std::string directory) {
	LOGD("Creating mutex.");
	updateMutex = Mutex();
	updateMutex.init("DMDirc-Updater");
	LOGD("Waiting for mutex.");
	updateMutex.lock();
	LOGD("Checking for old launcher.");
	deleteOldLauncher();
	bool relaunchNeeded = FALSE;
	LOGD("Checking launcher auto update = " << LAUNCHER_AUTOUPDATE);
	if (config.getBoolValue("launcher.autoupdate", LAUNCHER_AUTOUPDATE)) {
		LOGD("Attempting to update launcher.");
		int success = updateLauncher(directory, Utils::getExePath());
		LOGD("Update result: " << success);
		if (success == -1) {
			LOGD("Failed: attempting to update app data")
			success = updateLauncher(directory, Utils::GetAppDataDirectory());
			LOGD("Update result: " << success);
		}
		if (success == 1) {
			LOGD("Success, restart needed.");
			relaunchNeeded = TRUE;
		}
	}
	LOGD("Checking application update = " << APPLICATION_AUTOUPDATE);
	if (config.getBoolValue("application.autoupdate", APPLICATION_AUTOUPDATE)) {
		LOGD("Attempting to update application.");
		int success = updateApplication(directory, Utils::getExePath());
		if (success == -1) {
			LOGD("Failed: Updating to app data.");
			success = updateApplication(directory, Utils::GetAppDataDirectory());
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
		LOGD("Update file exists")
		file.close();
		LOGD("Attempting to backup existing launcher.");
		Updater::backupExistingLauncher();
		if (!Platform::moveFile(from + "/" + Utils::getExeName(), to + Utils::getExePath())) {
			LOGD("Moving new launcher failed.");
			return -1;
		}
		LOGD("Updating launcher suceeded.");
		return 1;
	}
	LOGD("Updating launcher not required.");
	return 0;
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
	for (unsigned int i = 0; i < files.size(); i++) {
		LOGD("Attempting to update: " << files[i]);
		std::string updateSource = "." + files[i];
		std::string updateTarget = files[i];
		std::ifstream file((char*)(from + "/" + updateSource).c_str());
		if (file.good()) {
			LOGD("Update file exists, trying to copy.");
			if (!Platform::moveFile(from + updateSource, to + updateTarget)) {
				LOGD("Updating failed.");
				return -1;
			}
		}
		else {
			LOGD("Update file does not exist.");
		}
		
	}
	return 1;
}

void Updater::relaunch() {
	updateMutex = Mutex();
	updateMutex.init("DMDirc-Updater");
	STARTUPINFO         sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(sInfo));
	sInfo.cb = sizeof(sInfo);
	ZeroMemory(&pInfo, sizeof(pInfo));
	LOGD("Creating new process.");
	CreateProcess((char*)Utils::getExePathAndName().c_str(),
		NULL, NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &sInfo, &pInfo);
	LOGD("Releasing mutex");
	updateMutex.unlock();
	LOGD("Exiting app.");
	ExitProcess(0);
}

void Updater::deleteOldLauncher() {
	std::string exeNameOld = Utils::getExePathAndName() + ".old";
	std::ifstream file(exeNameOld.c_str());
	if (file.good()) {
		LOGD("Old Launcher exists, deleting.");
		file.close();
		if (remove(exeNameOld.c_str()) != 0) {
			perror("Failed to delete the old launcher");
		}
	}
}

void Updater::backupExistingLauncher() {
	TCHAR buffer[MAX_PATH] = { 0 };
	DWORD bufSize = sizeof (buffer) / sizeof (*buffer);
	GetModuleFileName(NULL, buffer, bufSize);
	if (Platform::moveFile(Utils::getExePathAndName(), Utils::getExePathAndName() + ".old")) {
		LOGD("Unable to backup existing launcher.");
	}
}

bool Updater::isUpdateWaiting() {
	return !newVersion.empty();
}

std::string Updater::getNewVersion() {
	return newVersion;
}
