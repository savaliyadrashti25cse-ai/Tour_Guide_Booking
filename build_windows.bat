@echo off
setlocal
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  echo Install a C++17-capable MinGW-w64 compiler and add its bin folder to PATH.
  pause
  exit /b 1
)
if not exist Executable mkdir Executable
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -static -static-libgcc -static-libstdc++ Source\*.cpp -o Executable\Project.exe
if errorlevel 1 (
  echo Build failed. Read the compiler message above.
  pause
  exit /b 1
)
echo Build successful: Executable\Project.exe
pause
