include_directories(
        "$ENV{JAVA_HOME}"
        "$ENV{JAVA_HOME}include"
)

add_library(JVMLauncher JVMLauncher.cpp)
add_library(JVMLauncherUtils JVMLauncherUtils.cpp)
