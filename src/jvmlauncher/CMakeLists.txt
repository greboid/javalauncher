FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})
 
include_directories(${CMAKE_SOURCE_DIR}/src)
add_library(JVMLauncher JVMLauncher.cpp JVMLauncher.h JVMLauncherUtils.cpp JVMLauncherUtils.h)
if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()
