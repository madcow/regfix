@echo off
setlocal enabledelayedexpansion

rem locate msbuild installation path
for /f "usebackq tokens=*" %%i in (`%~dp0\vswhere.exe -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
	set "msbuild=%%i"
)

if not exist "%msbuild%" (
	echo Error: Cannot locate MSBuild.exe
	exit /b 1
)

rem run msbuild
"%msbuild%" %*