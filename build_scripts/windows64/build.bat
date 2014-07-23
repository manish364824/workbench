@echo off
if [%MACHINE%]==[] call setup_env.bat

set OLD_DIR=%CD%
REM rd /q/s %DIR%\caret7_source\build
md %DIR%\caret7_source\build
cd /d %DIR%\caret7_source\build
REM cmake -DFREETYPE_LIBRARY=C:\dev64\install\FreeType\lib\freetype240MT_D.lib -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release ../src
cmake -DFREETYPE_INCLUDE_DIR_freetype2=C:\dev64\install\FreeType-2.4.8\include\freetype2 -DFREETYPE_INCLUDE_DIR_ft2build=C:\dev64\install\FreeType-2.4.8\include  -DFREETYPE_LIBRARY=C:\dev64\install\FreeType-2.4.8\lib\freetype.lib -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release ../src
jom -j8
cd /d %OLD_DIR%
set OLD_DIR=
