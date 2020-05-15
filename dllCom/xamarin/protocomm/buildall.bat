@echo off
rem %comspec% /k "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

rem : script user pour les paths
rem : VSDEVCMD doit pointer vers VsDevCmd.bat
call buildall_user.bat

rem Positionner L'environnement de dev
call %VSDEVCMD%

rem Appeler le build pour toutes les cibles
MSBuild.exe /p:Configuration=Release /p:Platform="x86"
MSBuild.exe /p:Configuration=Release /p:Platform="x64"
MSBuild.exe /p:Configuration=Release /p:Platform="ARM"
MSBuild.exe /p:Configuration=Release /p:Platform="ARM64"
pause