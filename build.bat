@echo off
pushd %1
call cl -nologo -DNEBUG -O2 -W4 %1.c -Fe%1.exe -link -incremental:no
popd
