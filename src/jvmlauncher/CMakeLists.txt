FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})
 
include_directories(${CMAKE_SOURCE_DIR}/src)
add_library(JVMLauncher JVMLauncher.cpp JVMLauncher.h JVMLauncherUtils.cpp JVMLauncherUtils.h)