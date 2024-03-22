call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
call .\scripts\build-msvc.cmd
REM build the solution
devenv VulkanWorkspace.sln /Build "Release|x64"
