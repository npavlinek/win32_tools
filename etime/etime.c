#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fixed_width_integers.h"
#include "win32.h"

int main(int argc, const char** argv)
{
    Win32Fns win32 = { 0 };
    int64 freq, begin, end;
    char buf[4096] = { 0 };
    WIN32_STARTUPINFOA si = { 0 };
    WIN32_PROCESS_INFORMATION pi = { 0 };
    int i;
    double secs;

    if (argc < 2)
    {
        fprintf(stderr, "error: no command given\n");
        return 1;
    }

    Win32_LoadFunctions(&win32);

    win32.QueryPerformanceFrequency(&freq);

    for (i = 1; i < argc; ++i)
    {
        strcat_s(buf, 4096, argv[i]);
        strcat_s(buf, 4096, " ");
    }

    win32.QueryPerformanceCounter(&begin);

    si.cb = sizeof(WIN32_STARTUPINFOA);
    if (win32.CreateProcessA(NULL, buf, NULL, NULL, 0, 0, NULL, NULL, &si, &pi) == 0)
    {
        fprintf(stderr, "error: couldn't execute command\n");
        return 1;
    }
    win32.WaitForSingleObject(pi.hProcess, WIN32_INFINITE);

    win32.QueryPerformanceCounter(&end);
    secs = (double)(end - begin) / (double)freq;
    printf("Command took %.6f seconds\n", secs);
    return 0;
}
