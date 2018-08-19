@echo off

set ROOTDIR_QT=d:\Dev\src\trunk\ExtLib\Qt5\win32

set QTDIR=%ROOTDIR_QT%
REM set QMAKEFEATURES=%VPROOTDIR_CORE_BUILD%\qmake\mkspecs\features
set QMAKESPEC=win32-msvc

call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" x86

set Path=%ROOTDIR_QT%\bin;%Path%
set LIB=%ROOTDIR_QT%\lib;%LIB%
