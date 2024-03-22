call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"

.\vendor\premake\windows\premake5.exe  vs2022

REM build the solution
devenv VulkanWorkspace.sln /Build "Release|x64"
