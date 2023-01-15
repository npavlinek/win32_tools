#include <assert.h>
#include <stdint.h>

#include "win32.h"

#define ASSERT(cond)        \
    do {                    \
        if (!(cond))        \
            __debugbreak(); \
    } while (false)

static bool isWatching = true;

extern "C" {
void* memset(void* dest, int ch, size_t count);
}

static inline void zeroMemory(void* mem, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        reinterpret_cast<char*>(mem)[i] = 0;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-pragma-intrinsic"
#pragma function(memset)
#pragma clang diagnostic pop
void* memset(void* dest, int, size_t count)
{
    zeroMemory(dest, count);
    return dest;
}

static bool ctrlHandler(uint32_t ctrlType)
{
    switch (ctrlType) {
    case Win32::CTRL_C_EVENT:
        isWatching = false;
        return true;
    default:
        return false;
    }
}

static bool areStringsEqual(const char* lhs, const char* rhs)
{
    if (!lhs && !rhs) {
        // Technically they're equal. Maybe `false` would be better here?
        return true;
    }

    while (*lhs == *rhs++) {
        if (*lhs++ == '\0')
            return true;
    }

    return false;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-prototypes"
void mainCRTStartup()
#pragma clang diagnostic pop
{
    const wchar_t* cmdline = Win32::GetCommandLineW();
    int argCount = 0;
    wchar_t** args = Win32::CommandLineToArgvW(cmdline, &argCount);

    if (argCount <= 1) {
        // FIXME: Print error.
        Win32::ExitProcess(1);
    }

    Win32::SetConsoleCtrlHandler(ctrlHandler, true);

    enum { BUFFER_SIZE = 1024 };
    int cmdIndex = 0;
    char cmd[BUFFER_SIZE] = {};

    for (int i = 1; i < argCount; ++i) {
        const wchar_t* arg = args[i];
        // This is (perfectly?) fine assuming we're only dealing with ASCII, otherwise this won't
        // work correctly. In our case we'll most likely be dealing with only ASCII.
        while (*arg != L'\0') {
            if (cmdIndex < (BUFFER_SIZE - 1))
                cmd[cmdIndex++] = static_cast<char>(*arg & 0xff);
            ++arg;
        }
    }

    // Per documentation the argument string array should be freed using `LocalFree`.
    ASSERT(!Win32::LocalFree(args));

    // Just assume the buffer is big enough to store the entire path.
    char currentDirectory[BUFFER_SIZE];
    Win32::GetCurrentDirectoryA(BUFFER_SIZE, currentDirectory);

    void* dirHandle = Win32::CreateFileA(currentDirectory, Win32::FILE_LIST_DIRECTORY,
        Win32::FILE_SHARE_READ | Win32::FILE_SHARE_WRITE, nullptr, Win32::OPEN_EXISTING,
        Win32::FILE_FLAG_BACKUP_SEMANTICS | Win32::FILE_FLAG_OVERLAPPED, nullptr);
    ASSERT(dirHandle != Win32::INVALID_HANDLE_VALUE);

    while (isWatching) {
        char info[BUFFER_SIZE] = {};
        Win32::OVERLAPPED overlapped = {};
        Win32::ReadDirectoryChangesW(dirHandle, info, BUFFER_SIZE, true,
            Win32::FILE_NOTIFY_CHANGE_SIZE | Win32::FILE_NOTIFY_CHANGE_LAST_WRITE, nullptr,
            &overlapped, nullptr);

        uint32_t bytesTransferred = 0;
        if (Win32::GetOverlappedResultEx(dirHandle, &overlapped, &bytesTransferred, 1000, false) == 0) {
            switch (Win32::GetLastError()) {
            case Win32::WAIT_TIMEOUT:
                continue;
            }
        }

        const auto* fileInfo = reinterpret_cast<Win32::FILE_NOTIFY_INFORMATION*>(&info);
        char fileName[BUFFER_SIZE] = {};
        ASSERT(fileInfo->FileNameLength < (BUFFER_SIZE - 1));
        // Again, assuming we're dealing with only ASCII. This is a bit more of a leap because we're
        // dealing with file name which could have non-ASCII characters.
        for (uint32_t i = 0; i < fileInfo->FileNameLength; ++i)
            fileName[i] = static_cast<char>(fileInfo->FileName[i] & 0xff);

        // FIXME: This shouldn't be hard-coded, instead it should be read from `.gitignore` or
        // something. I don't know.
        if (fileName[0] == '.' || areStringsEqual(fileName, "out"))
            continue;

        Win32::STARTUPINFOA startupInfo = { .cb = sizeof(Win32::STARTUPINFOA) };
        Win32::PROCESS_INFORMATION processInfo = {};

        Win32::CreateProcessA(nullptr, cmd, nullptr, nullptr, false, 0, nullptr, nullptr,
            &startupInfo, &processInfo);
        Win32::WaitForSingleObject(processInfo.hProcess, Win32::INFINITE);
    }

    Win32::CloseHandle(dirHandle);
    Win32::ExitProcess(0);
}
