#include "Updater.h"

Updater::Updater(ConfigReader& config) {
    this->config = config;
}

void Updater::MoveRunningExecutable() {
    TCHAR buffer[MAX_PATH] = {0};
    DWORD bufSize = sizeof (buffer) / sizeof (*buffer);
    GetModuleFileName(NULL, buffer, bufSize);
    MoveFile(buffer, TEXT("old.exe"));
    CopyFile(TEXT("old.exe"), buffer, TRUE);
}
