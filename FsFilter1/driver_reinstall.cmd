@echo off

fltmc unload driver1

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection DefaultUninstall 132 .\driver1.inf
popd

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection DefaultInstall 132 .\driver1.inf
popd
