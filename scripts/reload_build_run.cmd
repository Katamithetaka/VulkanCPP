set script_dir=%~dp0
rem get root directory
set root_dir=%script_dir%..
cd %root_dir%

call .\scripts\reload_build.cmd %1

echo "===== Main.exe ====="
call .\build\bin\Debug\Main.exe
echo "===================="
