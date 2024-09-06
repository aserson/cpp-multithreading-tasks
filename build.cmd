@echo off
cd /d %~dp0

if not exist "external/googletest" (
    cd external
    git clone https://github.com/google/googletest.git

    cd googletest
    cmake .
    cmake --build . --config Release
)

if not exist "external/googletest" (
    cd external
    git clone https://github.com/google/googletest.git

    cd googletest
    mkdir build
    cd build
    cmake ..
    make
)

if not exist "build" (
    mkdir build
)

cd build

echo Generating build files with CMake...
cmake -G "Visual Studio 16 2019" -A x64 ..

echo Building the project...
cmake --build . --config Release