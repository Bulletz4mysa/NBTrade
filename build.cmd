@echo off

SET path=%path%;%SystemRoot%\Microsoft.NET\Framework\v2.0.50727;
SET BuildTarget=Build
SET BuildMode=Debug
if /i "%1"=="/release" set BuildMode=Release
if /i "%1"=="/rebuild" set BuildTarget=Rebuild
if /i "%2"=="/release" set BuildMode=Release
if /i "%2"=="/rebuild" set BuildTarget=Rebuild

if "%roboDeploy%" == "true"  GOTO ROBODEPLOY
echo Local Build
set BuildType=Local
msbuild build.proj /v:n
if errorlevel 1 pause
GOTO END

:ROBODEPLOY
	echo Robo Build
	set BuildType=RoboDeployer
	msbuild WebSolution.sln /v:n
:END