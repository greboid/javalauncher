cmake_minimum_required(VERSION 2.8)
project(JavaLauncher)

IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
  FIND_PACKAGE(Git)
  IF(GIT_FOUND)
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} describe --tags --always WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE "LAUNCHER_VERSION" ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  ELSE(GIT_FOUND)
    SET(LAUNCHER_VERSION 0)
  ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/src/version.h.in ${PROJECT_BINARY_DIR}/src/version.h)

set(APPLICATION_NAME DMDirc)
set(APPLICATION_PATH c:\\\\Program Files\\\\DMDirc\\\\)
set(APPLICATION_MAIN com\\\\dmdirc\\\\Main)
set(APPLICATION_SETUP com\\\\dmdric\\\\LauncherUtils)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config/ConfigDefaults.h.in"
  "${PROJECT_BINARY_DIR}/src/config/ConfigDefaults.h"
)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

set(JAVA_INCLUDE_PATH ${JNI_H})
set(JAVA_INCLUDE_PATH2 ${JNI_MD_H})
include_directories(
	"${JAVA_INCLUDE_PATH}"
	"${JAVA_INCLUDE_PATH2}"
)
set(RES_FILES "res/version.rc" "res/icon.rc")

add_subdirectory(src/config)
add_subdirectory(src/jvmlauncher)
add_subdirectory(src/singleinstance)
add_subdirectory(src/updater)
add_subdirectory(src/utils)

add_executable(javalauncher src/Main.cpp ${RES_FILES})
target_link_libraries (javalauncher Utils) 
target_link_libraries (javalauncher ConfigReader) 
target_link_libraries (javalauncher JVMLauncherUtils) 
target_link_libraries (javalauncher JVMLauncher) 
target_link_libraries (javalauncher SingleInstance) 
target_link_libraries (javalauncher Updater) 