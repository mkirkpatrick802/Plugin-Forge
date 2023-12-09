@echo off

echo Deleting Binaries...
rmdir /s /q .vs
rmdir /s /q Binaries
rmdir /s /q Intermediate
rmdir /s /q Saved

REM These are diffrent for every computer, so they need to be changed accordingly
set MyUVS="D:\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
set MyUBT="D:\Epic Games\UE_5.1\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"

set MyFullPath="%cd%\TPP_Plugin"

echo Regenerating Visual Studio project files...
%MyUVS% /projectfiles %MyFullPath%.uproject
%MyUBT% Development Win64 -Project=%MyFullPath%.uproject -TargetType=Editor -Progress -NoEngineChanges -NoHotReloadFromIDE

echo Opening Unreal Project...
%MyFullPath%.uproject

exit /b