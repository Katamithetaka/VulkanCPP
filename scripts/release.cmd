call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"

call .\vendor\premake\windows\premake5.exe  vs2022

REM build the solution.
msbuild VulkanWorkspace.sln /property:Configuration=Release
