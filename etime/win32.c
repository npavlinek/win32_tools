#include "win32.h"

void Win32_LoadFunctions(Win32Fns* win32)
{
    void* kernel32 = LoadLibraryA("kernel32.dll");

    win32->CreateProcessA = (Win32_CreateProcessA)GetProcAddress(kernel32, "CreateProcessA");
    win32->QueryPerformanceCounter = (Win32_QueryPerformanceCounter)GetProcAddress(kernel32, "QueryPerformanceCounter");
    win32->QueryPerformanceFrequency = (Win32_QueryPerformanceFrequency)GetProcAddress(kernel32, "QueryPerformanceFrequency");
    win32->WaitForSingleObject = (Win32_WaitForSingleObject)GetProcAddress(kernel32, "WaitForSingleObject");
}
