set script_dir=%~dp0
rem get root directory
set root_dir=%script_dir%..
cd %root_dir%

rem check number of args
if "%1"=="" (
    set build_type="Debug"
) else (
    set build_type=%1
)


call .\scripts\reload_build.cmd %build_type%

echo "===== Main.exe ====="
echo .\build\bin\%build_type%-windows-x86_64\Main\Main.exe
call .\build\bin\%build_type%-windows-x86_64\Main\Main.exe
echo "===================="
