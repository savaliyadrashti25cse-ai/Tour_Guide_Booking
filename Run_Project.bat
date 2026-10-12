@echo off
cd /d "%~dp0"
if not exist "Executable\Project.exe" (
  echo Executable not found. Run build_windows.bat first.
  pause
  exit /b 1
)
"Executable\Project.exe" --data-dir "%~dp0Data"
pause
