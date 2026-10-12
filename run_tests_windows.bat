@echo off
setlocal
cd /d "%~dp0"
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -ISource Source\Booking.cpp Source\FileManager.cpp Source\Guide.cpp Source\GuideSystem.cpp Source\Person.cpp Source\Tour.cpp Source\Tourist.cpp Source\Validation.cpp Tests\system_tests.cpp -o Tests\system_tests.exe
if errorlevel 1 exit /b 1
Tests\system_tests.exe
pause
