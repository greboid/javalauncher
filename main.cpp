#include "jvmlauncher/JVMLauncher.h"
#include "singleinstance/SingleInstance.h"
#include "config/ConfigReader.h"
#include "updater/Updater.h"
#include "utils/utils.h"
#include <windows.h>
#include <conio.h>

using namespace std;

int main(int argc, char** argv) {
    ConfigReader* config = new ConfigReader();
    SingleInstance* singleInstance = new SingleInstance(config);
    if (!singleInstance->getCanStart()) {
        cout << "Another instance already running." << endl;
        return EXIT_FAILURE;
    }
    std::vector<std::string> cliArgs = Utils::arrayToVector(argc, argv);
    cliArgs.erase(cliArgs.begin());
    cliArgs.push_back("-l");
    cliArgs.push_back("bob-1");
    cliArgs = Utils::mergeVectors(config->getVectorValue("application.args", std::vector<std::string>(0)),cliArgs);
    std::vector<std::string> jvmArgs;
    jvmArgs.push_back("-Dfile.encoding=utf-8");
    jvmArgs = Utils::mergeVectors(config->getVectorValue("jvm.args", std::vector<std::string>(0)), jvmArgs);
    JVMLauncher* launcher = new JVMLauncher(
        config->getStringValue("application.path", ".\\"),
        config->getStringValue("application.main", "com/dmdirc/Main"),
        jvmArgs, cliArgs, config);
    try {
        HANDLE handle = launcher->forkAndLaunch();
        WaitForSingleObject(handle, INFINITE);
    } catch (JVMLauncherException& ex) {
        cout << "Launching the JVM failed" << endl;
        cout << ex.what() << endl;
        cout << "Press any key to exit" << endl;
        getch();
    }
    singleInstance->stopped();
    return EXIT_SUCCESS;
}
