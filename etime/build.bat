@echo off
call cl -nologo -DNEBUG -O2 -W4 etime.c -Feetime.exe -link -incremental:no
