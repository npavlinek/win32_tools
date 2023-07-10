@echo off

if "%1"=="" (
    echo error: no installation directory specified
    exit /B 1
)

copy out\etime.exe "%1"
