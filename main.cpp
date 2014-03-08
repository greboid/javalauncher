#include "jvmlauncher/JVMLauncher.h"
#include "singleinstance/SingleInstance.h"
#include "config/ConfigReader.h"
#include "updater/Updater.h"
#include <windows.h>

using namespace std;

int main(int argc, char** argv) {
    ConfigReader* config = new ConfigReader();
    SingleInstance* singleInstance = new SingleInstance(config);
    if (!singleInstance->getCanStart()) {
        cout << "Another instance already running." << endl;
        return EXIT_FAILURE;
    }
    JVMLauncher* launcher = new JVMLauncher("C:\\Program Files\\DMDirc\\", "com/dmdirc/Main", config);
    try {
        HANDLE handle = launcher->forkAndLaunch();
        WaitForSingleObject(handle, INFINITE);
    } catch (JVMLauncherException& ex) {
        cout << "Launching the JVM failed" << endl;
        cout << ex.what() << endl;
        cout << "Press any key to exit" << endl;
        cin.ignore(1);
    }
    singleInstance->stopped();
    return EXIT_SUCCESS;
}
