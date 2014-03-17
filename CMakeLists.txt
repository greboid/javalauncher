cmake_minimum_required(VERSION 2.8)
project(JavaLauncher)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

set(JAVA_INCLUDE_PATH ${JNI_H})
set(JAVA_INCLUDE_PATH2 ${JNI_MD_H})
include_directories(
	"${JAVA_INCLUDE_PATH}"
	"${JAVA_INCLUDE_PATH2}"
)

add_subdirectory(src/config)
add_subdirectory(src/jvmlauncher)
add_subdirectory(src/singleinstance)
add_subdirectory(src/updater)
add_subdirectory(src/utils)

add_executable(javalauncher src/Main.cpp)
target_link_libraries (javalauncher ConfigReader) 
target_link_libraries (javalauncher JVMLauncherUtils) 
target_link_libraries (javalauncher JVMLauncher) 
target_link_libraries (javalauncher SingleInstance) 
target_link_libraries (javalauncher Updater) 
target_link_libraries (javalauncher Utils) 