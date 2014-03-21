include_directories(
        "$ENV{JAVA_HOME}"
        "$ENV{JAVA_HOME}include"
)

add_library(JVMLauncher JVMLauncher.cpp JVMLauncher.h JVMLauncherUtils.cpp JVMLauncherUtils.h)