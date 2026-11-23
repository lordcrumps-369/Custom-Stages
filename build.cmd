@echo off
setlocal
pushd "%~dp0"
if not exist work mkdir work
cl /nologo /c /GS- /GR- /EHs-c- /O2 /Oi- /Oy- /arch:IA32 /Fo"work\TS2-CustomStages.obj" TS2-CustomStages.cpp
if errorlevel 1 exit /b 1
lib /nologo /def:kernel32.def /machine:X86 /out:"work\kernel32_alias.lib"
if errorlevel 1 exit /b 1
link /nologo /dll /nodefaultlib /entry:DllMain /machine:X86 /out:TS2-CustomStages.asi /map:"work\TS2-CustomStages.map" "work\TS2-CustomStages.obj" "work\kernel32_alias.lib"
exit /b %errorlevel%
