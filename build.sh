#!/bin/bash
echo $JAVA_HOME
: ${JAVA_HOME:="/usr/lib/jvm/default-java/"}
echo $JAVA_HOME
export JAVA_HOME
if [ -d build ]
then
    rm -r build
fi
mkdir build && cd build && cmake -DCMAKE_TOOLCHAIN_FILE=./Toolchain-cross-mingw32-linux.cmake .. && make && cd ..
