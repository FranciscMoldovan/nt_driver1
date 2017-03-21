@REM
@REM Runs the DefaultInstall section of DRIVER1.INF
@REM

@echo off

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection DefaultInstall 132 .\driver1.inf
popd

