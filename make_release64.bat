echo off
echo making 64 bit



echo making Core2duo version
qmake  -o Makefile "QMAKESPEC=x86_64-pc-mingw32-gcc-4.3.0" eelsmodel.pro
mingw32-make -B release

mingw32-make clean