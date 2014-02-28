#include "SingleInstance.h"

#define SINGLEINSTANCE FALSE

using namespace std;

SingleInstance::SingleInstance(ConfigReader* config) {
    this->config = config;
}

SingleInstance::~SingleInstance() {
    if (instanceMutex) {
        stopped();
    }
}

bool SingleInstance::getCanStart() {
    if (SINGLEINSTANCE) {
    //if (config->getBoolValue("launcher.singleInstance", SINGLEINSTANCE)) {
        instanceMutex = CreateMutex(NULL, true, "DMDirc");
        if (instanceMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
            CloseHandle(instanceMutex);
            return false;
        }
    }
    return true;
}

void SingleInstance::stopped() {
    ReleaseMutex(instanceMutex);
}