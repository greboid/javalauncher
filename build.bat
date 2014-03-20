@echo off
IF "%JAVA_HOME%" == "" GOTO SETJAVAHOME
GOTO JAVAHOMEEXISTS
:SETJAVAHOME
	JAVA_HOME="C:\Program Files\Java\jre8"
:JAVAHOMEEXISTS
if EXIST "build" (
    rmdir /s/q build
)
mkdir build && cd build && cmake -G"NMake Makefiles" .. && nmake
cd ..
