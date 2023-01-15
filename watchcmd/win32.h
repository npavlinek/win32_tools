#pragma once

namespace Win32 {

static const void* INVALID_HANDLE_VALUE = reinterpret_cast<void*>(static_cast<intptr_t>(-1));

enum : uint32_t {
    CTRL_C_EVENT = 0,
    FILE_FLAG_BACKUP_SEMANTICS = 0x02000000,
    FILE_FLAG_OVERLAPPED = 0x40000000,
    FILE_LIST_DIRECTORY = 1,
    FILE_NOTIFY_CHANGE_LAST_WRITE = 0x10,
    FILE_NOTIFY_CHANGE_SIZE = 0x8,
    FILE_SHARE_READ = 0x1,
    FILE_SHARE_WRITE = 0x2,
    INFINITE = 0xffffffff,
    OPEN_EXISTING = 3,
    WAIT_TIMEOUT = 258,
};

struct FILE_NOTIFY_INFORMATION {
    uint32_t NextEntryOffset;
    uint32_t Action;
    uint32_t FileNameLength;
    wchar_t FileName[1];
};

struct OVERLAPPED {
    uintptr_t Internal;
    uintptr_t InternalHigh;
    union {
        struct {
            uint32_t Offset;
            uint32_t OffsetHigh;
        } DUMMYSTRUCTNAME;
        void* Pointer;
    } DUMMYUNIONNAME;
    void* hEvent;
};

struct PROCESS_INFORMATION {
    void* hProcess;
    void* hThread;
    uint32_t dwProcessId;
    uint32_t dwThreadId;
};

struct STARTUPINFOA {
    uint32_t cb;
    char* lpReserved;
    char* lpDesktop;
    char* lpTitle;
    uint32_t dwX;
    uint32_t dwY;
    uint32_t dwXSize;
    uint32_t dwYSize;
    uint32_t dwXCountChars;
    uint32_t dwYCountChars;
    uint32_t dwFillAttribute;
    uint32_t dwFlags;
    uint16_t wShowWindow;
    uint16_t cbReserved2;
    unsigned char* lpReserved2;
    void* hStdInput;
    void* hStdOutput;
    void* hStdError;
};

extern "C" {
__declspec(dllimport) bool CloseHandle(void* hObject);
__declspec(dllimport) bool CreateProcessA(const char* lpApplicationName, char* lpCommandLine, void* lpProcessAttributes, void* lpThreadAttributes, bool bInheritHandles, uint32_t dwCreationFlags, void* lpEnvironment, const char* lpCurrentDirectory, STARTUPINFOA* lpStartupInfo, PROCESS_INFORMATION* lpProcessInformation);
__declspec(dllimport) bool GetOverlappedResultEx(void* hFile, OVERLAPPED* lpOverlapped, uint32_t* lpNumberOfBytesTransferred, uint32_t dwMilliseconds, bool bAlertable);
__declspec(dllimport) bool ReadDirectoryChangesW(void* hDirectory, void* lpBuffer, uint32_t nBufferLength, bool bWatchSubtree, uint32_t dwNotifyFilter, uint32_t* lpBytesReturned, OVERLAPPED* lpOverlapped, void* lpCompletionRoutine);
__declspec(dllimport) bool SetConsoleCtrlHandler(bool (*HandlerRoutine)(uint32_t), bool Add);
__declspec(dllimport) uint32_t GetCurrentDirectoryA(uint32_t nBufferLength, char* lpBuffer);
__declspec(dllimport) uint32_t GetLastError();
__declspec(dllimport) uint32_t WaitForSingleObject(void* hHandle, uint32_t dwMilliseconds);
__declspec(dllimport) void ExitProcess(unsigned int uExitCode);
__declspec(dllimport) void* CreateFileA(const char* lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode, void* lpSecurityAttributes, uint32_t dwCreationDisposition, uint32_t dwFlagsAndAttributes, void* hTemplateFile);
__declspec(dllimport) void* LocalFree(void* hMem);
__declspec(dllimport) wchar_t* GetCommandLineW();
__declspec(dllimport) wchar_t** CommandLineToArgvW(const wchar_t* lpCmdLine, int* pNumArgs);
}

} // namespace Win32
