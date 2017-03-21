@REM
@REM Runs the BootInstall section of DRIVER1.INF
@REM

@echo off

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection BootInstall 132 .\driver1.inf
popd

