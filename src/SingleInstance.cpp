#include "SingleInstance.h"

using namespace std;

SingleInstance::SingleInstance(ConfigReader& config) {
    this->config = config;
}

SingleInstance::~SingleInstance() {
	BOOST_LOG_TRIVIAL(debug) << "Destroying single instance.";
    if (instanceMutex.unlock()) {
		BOOST_LOG_TRIVIAL(debug) << "Stopping instance.";
        stopped();
    }
}

bool SingleInstance::getCanStart() {
	BOOST_LOG_TRIVIAL(debug) << "Checking if we should use single instance.";
	if (config.getBoolValue("launcher.singleinstance", LAUNCHER_SINGLEINSTANCE)) {
		BOOST_LOG_TRIVIAL(debug) << "Creating single instance.";
		instanceMutex = Mutex();
		if (!instanceMutex.init("DMDirc")) {
			BOOST_LOG_TRIVIAL(debug) << "Single instance exists, we should not start.";
			stopped();
            return false;
		}
		else {
			BOOST_LOG_TRIVIAL(debug) << "Single instance does not exist.";
			return true;
		}
	}
	else {
		BOOST_LOG_TRIVIAL(debug) << "Single instance not set, we should start.";
		stopped();
		return true;
	}
}

void SingleInstance::stopped() {
	BOOST_LOG_TRIVIAL(debug) << "Releasing mutex.";
	instanceMutex.unlock();
}

