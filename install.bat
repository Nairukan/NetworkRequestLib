@echo off
rd /s /q build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cd build
mingw32-make
mingw32-make install
cd ..

rd /s /q build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cd build
mingw32-make
mingw32-make install
cd ..

@echo Build and installation completed!