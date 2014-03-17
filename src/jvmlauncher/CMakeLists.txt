find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

set(JAVA_INCLUDE_PATH ${JNI_H})
set(JAVA_INCLUDE_PATH2 ${JNI_MD_H})
include_directories(
	"${JAVA_INCLUDE_PATH}"
	"${JAVA_INCLUDE_PATH2}"
)

add_library(JVMLauncher JVMLauncher.cpp)
add_library(JVMLauncherUtils JVMLauncherUtils.cpp)
