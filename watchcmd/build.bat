@echo off
cls

set ccflags=^
-O2 ^
-Weverything ^
-Wno-c++98-compat ^
-Wno-c++98-compat-pedantic ^
-ffreestanding ^
-fno-exceptions ^
-fno-rtti ^
-fuse-ld=lld-link ^
-mno-stack-arg-probe ^
-pedantic ^
-std=c++20

set ldflags=^
-Xlinker -incremental:no ^
-Xlinker -nodefaultlib ^
-Xlinker -subsystem:console ^
-lkernel32 ^
-lshell32

if not exist out mkdir out
pushd out
call clang++ %ccflags% %ldflags% -o watchcmd.exe ..\watchcmd.cpp
popd
