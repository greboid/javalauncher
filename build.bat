@echo off
IF "%JAVA_HOME%" == "" GOTO SETJAVAHOME
GOTO JAVAHOMEEXISTS
:SETJAVAHOME
	JAVA_HOME="C:\Program Files\Java\jre8"
:JAVAHOMEEXISTS
mkdir build-x86
mkdir build-x64
cd build-x86 && cmake -G"Visual Studio 12" .. && msbuild.exe ALL_BUILD.vcxproj /p:Configuration=Release
cd ..
cd build-x64 && cmake -G"Visual Studio 12 Win64" .. && msbuild.exe ALL_BUILD.vcxproj /p:Configuration=Release
cd ..
