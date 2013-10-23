echo off
echo making Pentium 3 release

qmake -o Makefile eelsmodel.pro
mingw32-make -B release


echo making Core2duo version
qmake  -o Makefile "CONFIG+=core2duo" eelsmodel.pro
mingw32-make -B release

echo making i7 version
qmake  -o Makefile "CONFIG+=i7" eelsmodel.pro
mingw32-make -B release

mingw32-make clean