
set src_file_mask=%1
set dst_path=%2
set symbols_location=%3

echo "source file mask to copy: [%src_file_mask%]"
echo "destination folder: [%dst_path%]"
echo "symbols will be saved to location: [%symbols_location%]"

xcopy /Y %src_file_mask% %dst_path%
"c:\Program Files (x86)\Windows Kits\10\Debuggers\x86\symstore.exe"  add /f %src_file_mask% /s %symbols_location% /t "Driver"