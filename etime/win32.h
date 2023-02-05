#ifndef WIN32_H
#define WIN32_H

#include "fixed_width_integers.h"

#define BOOL int
#define DWORD uint32
#define FARPROC void*
#define HANDLE void*
#define HMODULE void*
#define LPBYTE unsigned char*
#define LPCSTR const char*
#define LPPROCESS_INFORMATION WIN32_PROCESS_INFORMATION*
#define LPSTARTUPINFOA WIN32_STARTUPINFOA*
#define LPSTR char*
#define LPVOID void*
#define WORD uint16

#define WIN32_INFINITE 4294967295

typedef struct WIN32_PROCESS_INFORMATION
{
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} WIN32_PROCESS_INFORMATION;

typedef struct WIN32_STARTUPINFOA
{
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} WIN32_STARTUPINFOA;

extern __declspec(dllimport) FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
extern __declspec(dllimport) HMODULE LoadLibraryA(LPCSTR lpLibFileName);

typedef BOOL (*Win32_CreateProcessA)(LPCSTR lpApplicationName, LPSTR lpCommandLine, void* lpProcessAttributes, void* lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL (*Win32_QueryPerformanceCounter)(int64* lpPerformanceCount);
typedef BOOL (*Win32_QueryPerformanceFrequency)(int64* lpFrequency);
typedef DWORD (*Win32_WaitForSingleObject)(HANDLE hHandle, DWORD dwMilliseconds);

typedef struct Win32Fns
{
    Win32_CreateProcessA CreateProcessA;
    Win32_QueryPerformanceCounter QueryPerformanceCounter;
    Win32_QueryPerformanceFrequency QueryPerformanceFrequency;
    Win32_WaitForSingleObject WaitForSingleObject;
} Win32Fns;

void Win32_LoadFunctions(Win32Fns* win32);

#undef BOOL
#undef DWORD
#undef FARPROC
#undef HANDLE
#undef HMODULE
#undef LPBYTE
#undef LPCSTR
#undef LPPROCESS_INFORMATION
#undef LPSTARTUPINFOA
#undef LPSTR
#undef LPVOID
#undef WORD

#endif /* WIN32_H */
