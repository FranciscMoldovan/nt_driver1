robocopy "\\vmware-host\shared folders\vm" c:\users\rciocas\desktop
cd c:\users\rciocas\desktop
call driver_stop.cmd 
call driver_reinstall.cmd 
call driver_start.cmd
call driver_query.cmd

pause

test1.exe
pause