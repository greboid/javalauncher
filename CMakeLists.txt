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
set(APPLICATION_PATH "c:\\\\Program Files\\\\DMDirc\\\\")
set(APPLICATION_MAIN "com/dmdirc/Main")
set(LAUNCHERUTILS_MAIN "com/dmdirc/LauncherUtils")
set(APPLICATION_SETUP "com/dmdric/LauncherUtils")
SET(APPLICATION_AUTOUPDATE TRUE)
SET(LAUNCHER_AUTOUPDATE TRUE)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config/ConfigDefaults.h.in"
  "${PROJECT_BINARY_DIR}/src/config/ConfigDefaults.h"
)

FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})

set(RES_FILES "${PROJECT_SOURCE_DIR}/res/version.rc" "${PROJECT_SOURCE_DIR}/res/icon.rc")

add_subdirectory(src/config)
add_subdirectory(src/jvmlauncher)
add_subdirectory(src/singleinstance)
add_subdirectory(src/updater)
add_subdirectory(src/utils)
add_subdirectory(src/log4z)
add_subdirectory(src/platform)

add_executable(javalauncher src/main.cpp ${RES_FILES})
target_link_libraries (javalauncher Utils)
target_link_libraries (javalauncher ConfigReader)
target_link_libraries (javalauncher JVMLauncher)
target_link_libraries (javalauncher SingleInstance)
target_link_libraries (javalauncher Updater)
target_link_libraries (javalauncher log4z)
target_link_libraries (javalauncher platform)

if(MSVC)
	set (CMAKE_EXE_LINKER_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /DEBUG")
	set (CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_MODULE_LINKER_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /DEBUG")
	set (CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_MODULE_LINKER_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /DEBUG")
	set (CMAKE_SHARED_LINKER_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /DEBUG")
	set (CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	set (CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup /DEBUG")
endif(MSVC)
