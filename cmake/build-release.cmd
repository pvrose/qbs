@echo off
setlocal

call "%~dp0cmake-msvc.cmd" --preset x64-release
if errorlevel 1 exit /b %errorlevel%

call "%~dp0cmake-msvc.cmd" --build "%~dp0..\out\build\x64-release" --config Release
exit /b %errorlevel%