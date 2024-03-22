@echo off
call .\scripts\build-msvc.cmd > build.log

REM check if devenv is in path
call devenv > nul
if %errorlevel% neq 0 (
    call cls
    echo "devenv not found in path"
    echo "Going to try using 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'"

    call :run_vs2022devcmd
)



REM build the solution
devenv VulkanWorkspace.sln /Build "Debug|x64" >> build.log

REM check if the build was successful
if %errorlevel% neq 0 (
    echo "Failed to build the solution, check .\build.log"
    exit /b 1
)

REM run the build/Debug/Main.exe
call cls

echo "Build successful"
echo "===== Main.exe ====="
call .\build\bin\Debug\Main.exe
echo "===================="

goto :eof


:run_vs2022devcmd

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if %errorlevel% neq 0 (
    echo "Failed to run VsDevCmd.bat: %errorlevel%"
    echo "Going to try using 'C:\Program Files\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat'"

    call :run_vs2019devcmd
)

exit /b 0

:run_vs2019devcmd

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
if %errorlevel% neq 0 (
    echo "Failed to run VsDevCmd.bat: %errorlevel%"
    exit 1
)

