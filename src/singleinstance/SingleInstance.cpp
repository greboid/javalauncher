#include "SingleInstance.h"

#define SINGLEINSTANCE TRUE
#define SETTING "launcher.singleinstance"

using namespace std;

SingleInstance::SingleInstance(ConfigReader& config) {
    this->config = config;
}

SingleInstance::~SingleInstance() {
	LOGD("Destroying single instance.");
    if (instanceMutex.unlock()) {
		LOGD("Stopping instance.");
        stopped();
    }
}

bool SingleInstance::getCanStart() {
	LOGD("Checking if we should use single instance.");
	if (config.getBoolValue(SETTING, SINGLEINSTANCE)) {
		LOGD("Creating single instance.");
		instanceMutex = Mutex();
		if (!instanceMutex.init("DMDirc")) {
			LOGD("Single instance exists, we should not start.");
			stopped();
            return false;
		}
		else {
			LOGD("Single instance does not exist.");
			return true;
		}
	}
	else {
		LOGD("Single instance not set, we should start.");
		stopped();
		return true;
	}
}

void SingleInstance::stopped() {
	LOGD("Releasing mutex.");
	instanceMutex.unlock();
}

