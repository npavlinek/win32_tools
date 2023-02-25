#include "win32.h"

void LoadWin32Functions(win32_t* win32)
{
    void* kernel32 = LoadLibraryA("kernel32.dll");

    win32->CloseHandle = (Win32_CloseHandle)GetProcAddress(kernel32, "CloseHandle");
    win32->CreateProcessA = (Win32_CreateProcessA)GetProcAddress(kernel32, "CreateProcessA");
    win32->GetExitCodeProcess = (Win32_GetExitCodeProcess)GetProcAddress(kernel32, "GetExitCodeProcess");
    win32->GetOverlappedResultEx = (Win32_GetOverlappedResultEx)GetProcAddress(kernel32, "GetOverlappedResultEx");
    win32->ReadDirectoryChangesW = (Win32_ReadDirectoryChangesW)GetProcAddress(kernel32, "ReadDirectoryChangesW");
    win32->SetConsoleCtrlHandler = (Win32_SetConsoleCtrlHandler)GetProcAddress(kernel32, "SetConsoleCtrlHandler");
    win32->GetCurrentDirectoryA = (Win32_GetCurrentDirectoryA)GetProcAddress(kernel32, "GetCurrentDirectoryA");
    win32->GetLastError = (Win32_GetLastError)GetProcAddress(kernel32, "GetLastError");
    win32->WaitForSingleObject = (Win32_WaitForSingleObject)GetProcAddress(kernel32, "WaitForSingleObject");
    win32->CreateFileA = (Win32_CreateFileA)GetProcAddress(kernel32, "CreateFileA");
}
