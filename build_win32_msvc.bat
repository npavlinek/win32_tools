@echo off

rd /S /Q out 2> NUL
mkdir out
pushd out

set compiler_flags=-Oi -W4 -nologo -wd4068
set linker_flags=-incremental:no

if "%1"=="optimized" (
    set compiler_flags=%compiler_flags% -MT -O2
) else (
    set compiler_flags=%compiler_flags% -MTd -Zi
    set linker_flags=%linker_flags% -debug:full
)

cl %compiler_flags% -Feetime -Tc..\etime.c -link %linker_flags%

popd
