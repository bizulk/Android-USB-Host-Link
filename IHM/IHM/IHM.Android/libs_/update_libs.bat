@echo off
rem Mise à jour des DLL pandavcom
rem Une simple copie depuis le répertoire projet dans dll/Xamarin.

rem ***************************************************************************
rem variables
rem ***************************************************************************

rem Chemin vers le dépôt 'AndroidFilaire' (todo : utiliser la ligne de commande ou verifier la présence dans l'environnement)
call update_libs_env_user.bat
rem Veut-on copier Debug ou Release
set BUILD=Release
set DLL_NAME=libdllcom_Android.so

rem ***************************************************************************
rem constantes
rem ***************************************************************************

rem chemin vers le répertoire racine des DLL
set DLLCOM_BIN_PATH=%ATO_WORKSPACE_PATH%\AndroidFilaire\dllcom\xamarin\protocomm\dllcom.Android\
 
set SRC_DLL_ARM64_PATH=%DLLCOM_BIN_PATH%\ARM64\%BUILD%\%DLL_NAME%
set SRC_DLL_ARM_PATH=%DLLCOM_BIN_PATH%\ARM\%BUILD%\%DLL_NAME%
set SRC_DLL_x64_PATH=%DLLCOM_BIN_PATH%\x64\%BUILD%\%DLL_NAME%
set SRC_DLL_x86_PATH=%DLLCOM_BIN_PATH%\x86\%BUILD%\%DLL_NAME%

rem Le nom des répertoires d'architecture est récupéré des manifest de VS
set TRGT_DLL_ARM64_PATH=.\arm64-v8a\%DLL_NAME%
set TRGT_DLL_ARM_PATH=.\armeabi-v7a\%DLL_NAME%
set TRGT_DLL_x64_PATH=.\x86_64\%DLL_NAME%
set TRGT_DLL_x86_PATH=.\x86\%DLL_NAME%

rem ***************************************************************************
rem constantes
rem ***************************************************************************
echo on
copy %SRC_DLL_ARM64_PATH% %TRGT_DLL_ARM64_PATH%
copy %SRC_DLL_ARM_PATH% %TRGT_DLL_ARM_PATH% 
copy %SRC_DLL_x64_PATH% %TRGT_DLL_x64_PATH%
copy %SRC_DLL_x86_PATH% %TRGT_DLL_x86_PATH%
