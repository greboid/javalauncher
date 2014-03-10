#include "Updater.h"

Updater::Updater(ConfigReader& config) {
    this->config = config;
    this->newVersion = "";
}

void Updater::doUpdate() {
    Updater::deleteOldLauncher();
    if (Updater::isUpdateWaiting()) {
        Updater::backupExistingLauncher();
    }
}

void Updater::deleteOldLauncher() {
    std::string exeNameOld = Utils::getExePathAndName()+ ".old";
    std::ifstream file (exeNameOld.c_str());
    if (file.good()) {
        file.close();
        if (remove(exeNameOld.c_str()) != 0) {
            perror("Failed to delete the old launcher");
        }
    }
}

void Updater::backupExistingLauncher() {
    TCHAR buffer[MAX_PATH] = {0};
    DWORD bufSize = sizeof (buffer) / sizeof (*buffer);
    GetModuleFileName(NULL, buffer, bufSize);
    MoveFile(Utils::getExePathAndName().c_str(), (Utils::getExePathAndName() + ".old").c_str());
}

bool Updater::isUpdateWaiting() {
    return !newVersion.empty();
}

std::string Updater::getNewVersion() {
    return newVersion;
}
