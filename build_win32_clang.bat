@echo off

rd /S /Q out 2> NUL
mkdir out
pushd out

set compiler_flags=-Weverything -pedantic -std=c89
set linker_flags=-fuse-ld=lld-link

if "%1"=="optimized" (
    set compiler_flags=%compiler_flags% -O2
) else (
    set compiler_flags=%compiler_flags% -g
)

clang %compiler_flags% -o etime.exe ..\etime.c %linker_flags%

popd
