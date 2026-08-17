@echo off
setlocal

call "%~dp0cmake-msvc.cmd" --preset x64-debug
if errorlevel 1 exit /b %errorlevel%

call "%~dp0cmake-msvc.cmd" --build "%~dp0..\out\build\x64-debug" --config Debug
exit /b %errorlevel%