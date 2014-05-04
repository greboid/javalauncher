#include "../log4z/log4z.h"
#include "SingleInstance.h"

#define SINGLEINSTANCE FALSE
#define SETTING "launcher.singleinstance"

using namespace std;

SingleInstance::SingleInstance(ConfigReader& config) {
    this->config = config;
}

SingleInstance::~SingleInstance() {
	LOGD("Destroying single instance.");
    if (instanceMutex) {
		LOGD("Stopping instance.");
        stopped();
    }
}

bool SingleInstance::getCanStart() {
	LOGD("Checking if we should use single instance.");
	if (config.getBoolValue(SETTING, SINGLEINSTANCE)) {
		LOGD("Creating single instance.");
        instanceMutex = CreateMutex(NULL, true, TEXT("DMDirc"));
        if (instanceMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
			LOGD("Single instance exists, we should not start.");
			stopped();
            return false;
        }
    }
	LOGD("Single instance not required or set, we should start.");
	stopped();
    return true;
}

void SingleInstance::stopped() {
	LOGD("Releasing mutex.");
    ReleaseMutex(instanceMutex);
	LOGD("Stopping mutex.");
	CloseHandle(instanceMutex);
}

