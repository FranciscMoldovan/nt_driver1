@echo off
:: 2011/08/10, SUN

:: --- save current directory & change to root of this script ---
pushd %CD%
cd %~dp0%

::
:: --- get CFGDIRS settings ---
::
if [%config_dirs_set%]==[1] goto cfgdirs_ok
if EXIST cfgdirs.cmd call cfgdirs.cmd
if [%config_dirs_set%]==[1] goto cfgdirs_ok
if EXIST ..\cfgdirs.cmd call ..\cfgdirs.cmd
if NOT [%config_dirs_set%]==[1] failmsg.cmd "CFGDIRS failed, stop MAKEKIT..."
if NOT [%ERRORLEVEL%]==[0] failmsg.cmd "CFGDIRS failed, stop PBUILD..." %ERRORLEVEL%
:cfgdirs_ok


::
:: configure project names
::
set ssdrv=DRIVER1
set ssdll=DLL1
set sstst=TEST1


::
:: --- store symbols (server) ---
::
if /I [%sym_server%]==[__EMPTY__] goto sym_store_not_found

:: DRIVER
if [%ssdrv%]==[__EMPTY__] goto skip_drv
echo ___
echo Store symbols for %ssdrv%...
%symstore_exe% add /f %rootdir%bin\Win32\Debug\%ssdrv%.sys /s %sym_server% /t %ssdrv%.sys
%symstore_exe% add /f %rootdir%bin\Win32\Debug\%ssdrv%.pdb /s %sym_server% /t %ssdrv%.pdb
%symstore_exe% add /f %rootdir%bin\Win32\Release\%ssdrv%.sys /s %sym_server% /t %ssdrv%.sys
%symstore_exe% add /f %rootdir%bin\Win32\Release\%ssdrv%.pdb /s %sym_server% /t %ssdrv%.pdb
%symstore_exe% add /f %rootdir%bin\x64\Debug\%ssdrv%.sys /s %sym_server% /t %ssdrv%.sys
%symstore_exe% add /f %rootdir%bin\x64\Debug\%ssdrv%.pdb /s %sym_server% /t %ssdrv%.pdb
%symstore_exe% add /f %rootdir%bin\x64\Release\%ssdrv%.sys /s %sym_server% /t %ssdrv%.sys
%symstore_exe% add /f %rootdir%bin\x64\Release\%ssdrv%.pdb /s %sym_server% /t %ssdrv%.pdb
:skip_drv

:: DLL
if [%ssdll%]==[__EMPTY__] goto skip_dll
echo ___
echo Store symbols for %ssdll%...
%symstore_exe% add /f %rootdir%bin\Win32\Debug\%ssdll%.dll /s %sym_server% /t %ssdll%.dll
%symstore_exe% add /f %rootdir%bin\Win32\Debug\%ssdll%.pdb /s %sym_server% /t %ssdll%.pdb
%symstore_exe% add /f %rootdir%bin\Win32\Release\%ssdll%.dll /s %sym_server% /t %ssdll%.dll
%symstore_exe% add /f %rootdir%bin\Win32\Release\%ssdll%.pdb /s %sym_server% /t %ssdll%.pdb
%symstore_exe% add /f %rootdir%bin\x64\Debug\%ssdll%.dll /s %sym_server% /t %ssdll%.dll
%symstore_exe% add /f %rootdir%bin\x64\Debug\%ssdll%.pdb /s %sym_server% /t %ssdll%.pdb
%symstore_exe% add /f %rootdir%bin\x64\Release\%ssdll%.dll /s %sym_server% /t %ssdll%.dll
%symstore_exe% add /f %rootdir%bin\x64\Release\%ssdll%.pdb /s %sym_server% /t %ssdll%.pdb
:skip_dll

:: TEST
if [%sstst%]==[__EMPTY__] goto skip_tst
echo ___
echo Store symbols for %sstst%...
%symstore_exe% add /f %rootdir%bin\Win32\Debug\%sstst%.exe /s %sym_server% /t %sstst%.exe
%symstore_exe% add /f %rootdir%bin\Win32\Debug\%sstst%.pdb /s %sym_server% /t %sstst%.pdb
%symstore_exe% add /f %rootdir%bin\Win32\Release\%sstst%.exe /s %sym_server% /t %sstst%.exe
%symstore_exe% add /f %rootdir%bin\Win32\Release\%sstst%.pdb /s %sym_server% /t %sstst%.pdb
%symstore_exe% add /f %rootdir%bin\x64\Debug\%sstst%.exe /s %sym_server% /t %sstst%.exe
%symstore_exe% add /f %rootdir%bin\x64\Debug\%sstst%.pdb /s %sym_server% /t %sstst%.pdb
%symstore_exe% add /f %rootdir%bin\x64\Release\%sstst%.exe /s %sym_server% /t %sstst%.exe
%symstore_exe% add /f %rootdir%bin\x64\Release\%sstst%.pdb /s %sym_server% /t %sstst%.pdb
:skip_tst

goto end


:sym_store_not_found
echo ___
echo WARNING, symbol server NOT set (%sym_server%), so we do NOT store any symbols...
set errorlevel 1
goto end

:end
popd
exit /b %ERRORLEVEL%