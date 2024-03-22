call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"


set premake=.\vendor\premake\windows\premake5.exe 
rem get script directory
set script_dir=%~dp0
rem get root directory
set root_dir=%script_dir%..
cd %root_dir%

call "%premake%" vs2022

REM build the solution
devenv VulkanWorkspace.sln /Build "Release|x64"
