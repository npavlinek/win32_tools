#ifndef WIN32_H
#define WIN32_H

#include <stddef.h>

#include "fixed_width_integers.h"

#define BOOL int
#define DWORD uint32
#define FARPROC void*
#define HANDLE void*
#define HMODULE void*
#define LPBYTE unsigned char*
#define LPCSTR const char*
#define LPDWORD uint32*
#define LPOVERLAPPED WIN32_OVERLAPPED*
#define LPPROCESS_INFORMATION WIN32_PROCESS_INFORMATION*
#define LPSTARTUPINFOA WIN32_STARTUPINFOA*
#define LPSTR char*
#define LPVOID void*
#define PVOID void*
#define ULONG_PTR uintptr
#define WCHAR wchar_t
#define WORD uint16

#define WIN32_CTRL_C_EVENT 0
#define WIN32_FILE_FLAG_BACKUP_SEMANTICS 33554432
#define WIN32_FILE_FLAG_OVERLAPPED 1073741824
#define WIN32_FILE_LIST_DIRECTORY 1
#define WIN32_FILE_NOTIFY_CHANGE_LAST_WRITE 16
#define WIN32_FILE_NOTIFY_CHANGE_SIZE 8
#define WIN32_FILE_SHARE_READ 1
#define WIN32_FILE_SHARE_WRITE 2
#define WIN32_INFINITE 4294967295
#define WIN32_INVALID_HANDLE_VALUE -1
#define WIN32_OPEN_EXISTING 3
#define WIN32_WAIT_TIMEOUT 258

typedef struct WIN32_FILE_NOTIFY_INFORMATION
{
    DWORD NextEntryOffset;
    DWORD Action;
    DWORD FileNameLength;
    WCHAR FileName[1];
} WIN32_FILE_NOTIFY_INFORMATION;

typedef struct WIN32_OVERLAPPED
{
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union
    {
        struct
        {
            DWORD Offset;
            DWORD OffsetHigh;
        } DUMMYSTRUCTNAME;
        PVOID Pointer;
    } DUMMYUNIONNAME;
    HANDLE hEvent;
} WIN32_OVERLAPPED;

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

typedef BOOL (*Win32_CloseHandle)(HANDLE hObject);
typedef BOOL (*Win32_CreateProcessA)(LPCSTR lpApplicationName, LPSTR lpCommandLine, void* lpProcessAttributes, void* lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL (*Win32_GetExitCodeProcess)(HANDLE hProcess, LPDWORD lpExitCode);
typedef BOOL (*Win32_GetOverlappedResultEx)(HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, DWORD dwMilliseconds, BOOL bAlertable);
typedef BOOL (*Win32_ReadDirectoryChangesW)(HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, void* lpCompletionRoutine);
typedef BOOL (*Win32_SetConsoleCtrlHandler)(BOOL (*HandlerRoutine)(DWORD dwCtrlType), BOOL Add);
typedef DWORD (*Win32_GetCurrentDirectoryA)(DWORD nBufferLength, LPSTR lpBuffer);
typedef DWORD (*Win32_GetLastError)(void);
typedef DWORD (*Win32_WaitForSingleObject)(HANDLE hHandle, DWORD dwMilliseconds);
typedef HANDLE (*Win32_CreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

typedef struct win32_s
{
    Win32_CloseHandle CloseHandle;
    Win32_CreateProcessA CreateProcessA;
    Win32_GetExitCodeProcess GetExitCodeProcess;
    Win32_GetOverlappedResultEx GetOverlappedResultEx;
    Win32_ReadDirectoryChangesW ReadDirectoryChangesW;
    Win32_SetConsoleCtrlHandler SetConsoleCtrlHandler;
    Win32_GetCurrentDirectoryA GetCurrentDirectoryA;
    Win32_GetLastError GetLastError;
    Win32_WaitForSingleObject WaitForSingleObject;
    Win32_CreateFileA CreateFileA;
} win32_t;

void LoadWin32Functions(win32_t* win32);

#undef BOOL
#undef DWORD
#undef FARPROC
#undef HANDLE
#undef HMODULE
#undef LPBYTE
#undef LPCSTR
#undef LPDWORD
#undef LPOVERLAPPED
#undef LPPROCESS_INFORMATION
#undef LPSTARTUPINFOA
#undef LPSTR
#undef LPVOID
#undef PVOID
#undef ULONG_PTR
#undef WCHAR
#undef WORD

#endif /* WIN32_H */
