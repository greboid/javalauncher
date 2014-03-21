FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})

add_library(JVMLauncher JVMLauncher.cpp JVMLauncher.h JVMLauncherUtils.cpp JVMLauncherUtils.h)