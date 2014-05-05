#include "platform.h"

using namespace std;

Platform::Platform() {
}

void Platform::createConsole() {
#ifdef WIN32
	LOGD("Allocationg a console.");
	AllocConsole();
	LOGD("Attaching a console.");
	AttachConsole(GetCurrentProcessId());
	FILE *conin, *conout;
	freopen_s(&conin, "conin$", "r", stdin);
	freopen_s(&conout, "conout$", "w", stdout);
	freopen_s(&conout, "conout$", "w", stderr);
#endif
}

void Platform::launchApplication(string application, char** arguments) {
#ifdef WIN32
	ShellExecute(NULL, LPSTR("open"), LPSTR(application.c_str()), LPSTR(arguments), NULL, SW_SHOWNORMAL);
#endif
}