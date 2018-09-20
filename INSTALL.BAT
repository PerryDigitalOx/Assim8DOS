@echo off
echo Assimilation Installation in progress...
md %1\assimil8
cd %1\assimil8
copy game10.exe %1 >NUL
%1
game10.exe >NUL
del game10.exe >NUL
echo Installation Complete
echo Type "GO" to run Assimilation.
