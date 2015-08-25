#include "SingleInstance.h"

using namespace std;

SingleInstance::SingleInstance(boost::program_options::variables_map& config) {
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
	BOOST_LOG_TRIVIAL(debug) << "Value: " << config["launcher.singleinstance"].as<bool>();
	if (config["launcher.singleinstance"].as<bool>()) {
		BOOST_LOG_TRIVIAL(debug) << "Creating single instance.";
		instanceMutex = Mutex();
		if (!instanceMutex.init(config["application.name"].as<string>())) {
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

