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
    JVMLauncher* launcher = new JVMLauncher(config->getStringValue("application.path", "."),
        config->getStringValue("application.main", "com/dmdirc/Main"), "", "", config);
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
