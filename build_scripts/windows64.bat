@echo=off
set CMD=c:\Windows\system32\cmd.exe
REM
REM Go to correct directory
REM
set BUILD_DIR=c:\dev7\windows64
cd %BUILD_DIR%

REM
REM Go into source directory
REM
cd caret7_source

echo "Caret7 Windows64 Build Result"

REM
REM Grab the latest Sources
REM
c:\cygwin\bin\git.exe reset --hard HEAD
c:\cygwin\bin\git.exe fetch
c:\cygwin\bin\git.exe reset --hard origin/master

REM
REM Build caret
REM
cd build_scripts/windows64
%CMD% /c build.bat
cd ..
cd ..
set DIST_DIR=caret@myelin1:/mainpool/storage/distribution/caret7_distribution/workbench/bin_windows64

echo "Copying Files"
c:\cygwin\bin\scp build/Desktop/wb_view.exe %DIST_DIR%
c:\cygwin\bin\scp build/CommandLine/wb_command.exe %DIST_DIR%
echo "Finished Copying Files"
