#include "SingleInstance.h"

#define SINGLEINSTANCE FALSE
#define SETTING "launcher.singleinstance"

using namespace std;

SingleInstance::SingleInstance(ConfigReader& config) {
    this->config = config;
}

SingleInstance::~SingleInstance() {
    if (instanceMutex) {
        stopped();
    }
}

bool SingleInstance::getCanStart() {
	if (config.getBoolValue(SETTING, SINGLEINSTANCE)) {
        instanceMutex = CreateMutex(NULL, true, TEXT("DMDirc"));
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

