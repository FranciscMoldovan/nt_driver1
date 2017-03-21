@REM
@REM Runs the DefaultUninstall section of DRIVER1.INF
@REM

@echo off

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection DefaultUninstall 132 .\driver1.inf
popd

