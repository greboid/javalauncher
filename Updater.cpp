#include "Updater.h"

void Updater::MoveRunningExecutable() {
    TCHAR buffer[MAX_PATH] = {0};
    DWORD bufSize = sizeof (buffer) / sizeof (*buffer);
    GetModuleFileName(NULL, buffer, bufSize);
    MoveFile(buffer, "old.exe");
    CopyFile("old.exe", buffer, TRUE);
}
