@echo off
setlocal
pushd "%~dp0"
if not exist work mkdir work
lib /nologo /def:"tests\kernel32_tests.def" /machine:X86 /out:"work\kernel32_tests.lib"
if errorlevel 1 exit /b 1
cl /nologo /c /GS- /GR- /EHs-c- /O2 /Oi- /Oy- /arch:IA32 /Fo"work\gameplay-tests.obj" "tests\gameplay.cpp"
if errorlevel 1 exit /b 1
link /nologo /subsystem:console /nodefaultlib /entry:TestStart /machine:X86 /out:"work\gameplay-tests.exe" "work\gameplay-tests.obj" "work\kernel32_tests.lib"
if errorlevel 1 exit /b 1
work\gameplay-tests.exe
if errorlevel 1 exit /b %errorlevel%
if /i not "%~1"=="startup" exit /b 0
cl /nologo /c /GS- /GR- /EHs-c- /O2 /Oi- /Oy- /arch:IA32 /Fo"work\startup-tests.obj" "tests\startup.cpp"
if errorlevel 1 exit /b 1
link /nologo /subsystem:console /nodefaultlib /entry:TestStart /machine:X86 /out:"work\startup-tests.exe" "work\startup-tests.obj" "work\kernel32_tests.lib"
if errorlevel 1 exit /b 1
work\startup-tests.exe
exit /b %errorlevel%
