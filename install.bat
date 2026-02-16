@echo off
if exist build rd /s /q build
cmake -S src -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
pause
