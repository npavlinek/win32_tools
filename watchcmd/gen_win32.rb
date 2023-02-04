require 'erb'

aliases = {
  BOOL: 'int',
  DWORD: 'uint32',
  FARPROC: 'void*',
  HANDLE: 'void*',
  HMODULE: 'void*',
  LPBYTE: 'unsigned char*',
  LPCSTR: 'const char*',
  LPDWORD: 'uint32*',
  LPOVERLAPPED: 'WIN32_OVERLAPPED*',
  LPPROCESS_INFORMATION: 'WIN32_PROCESS_INFORMATION*',
  LPSTARTUPINFOA: 'WIN32_STARTUPINFOA*',
  LPSTR: 'char*',
  LPVOID: 'void*',
  PVOID: 'void*',
  ULONG_PTR: 'uintptr',
  WCHAR: 'wchar_t',
  WORD: 'uint16',
}

defines = {
  CTRL_C_EVENT: 0,
  FILE_FLAG_BACKUP_SEMANTICS: 0x02000000,
  FILE_FLAG_OVERLAPPED: 0x40000000,
  FILE_LIST_DIRECTORY: 1,
  FILE_NOTIFY_CHANGE_LAST_WRITE: 0x10,
  FILE_NOTIFY_CHANGE_SIZE: 0x8,
  FILE_SHARE_READ: 0x1,
  FILE_SHARE_WRITE: 0x2,
  INFINITE: 0xffffffff,
  INVALID_HANDLE_VALUE: -1,
  OPEN_EXISTING: 3,
  WAIT_TIMEOUT: 258,
}

types = <<~EOF
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
EOF

functions = [
  ['kernel32', 'BOOL', 'CloseHandle', 'HANDLE hObject'],
  ['kernel32', 'BOOL', 'CreateProcessA', 'LPCSTR lpApplicationName, LPSTR lpCommandLine, void* lpProcessAttributes, void* lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation'],
  ['kernel32', 'BOOL', 'GetOverlappedResultEx', 'HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, DWORD dwMilliseconds, BOOL bAlertable'],
  ['kernel32', 'BOOL', 'ReadDirectoryChangesW', 'HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, void* lpCompletionRoutine'],
  ['kernel32', 'BOOL', 'SetConsoleCtrlHandler', 'BOOL (*HandlerRoutine)(DWORD dwCtrlType), BOOL Add'],
  ['kernel32', 'DWORD', 'GetCurrentDirectoryA', 'DWORD nBufferLength, LPSTR lpBuffer'],
  ['kernel32', 'DWORD', 'GetLastError', 'void'],
  ['kernel32', 'DWORD', 'WaitForSingleObject', 'HANDLE hHandle, DWORD dwMilliseconds'],
  ['kernel32', 'HANDLE', 'CreateFileA', 'LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile'],
]

win32_h_template = <<~EOF
  #ifndef WIN32_H
  #define WIN32_H

  #include <stddef.h>

  #include "fixed_width_integers.h"

  <% aliases.each do |key, val| -%>
  #define <%= key %> <%= val %>
  <% end -%>

  <% defines.each do |key, val| -%>
  #define WIN32_<%= key %> <%= val %>
  <% end -%>

  <%= types -%>

  extern __declspec(dllimport) FARPROC GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
  extern __declspec(dllimport) HMODULE LoadLibraryA(LPCSTR lpLibFileName);

  <% functions.each do |func| -%>
  typedef <%= func[1] %> (*Win32_<%= func[2] %>)(<%= func[3] %>);
  <% end -%>

  typedef struct win32_s
  {
  <% functions.each do |func| -%>
      Win32_<%= func[2] %> <%= func[2] %>;
  <% end -%>
  } win32_t;

  void LoadWin32Functions(win32_t* win32);

  <% aliases.each_key do |key| -%>
  #undef <%= key %>
  <% end -%>

  #endif /* WIN32_H */
EOF

win32_c_template = <<~EOF
  #include "win32.h"

  void LoadWin32Functions(win32_t* win32)
  {
      void* kernel32 = LoadLibraryA("kernel32.dll");

  <% functions.each do |func| -%>
      win32-><%= func[2] %> = (Win32_<%= func[2] %>)GetProcAddress(<%= func[0] %>, "<%= func[2] %>");
  <% end -%>
  }
EOF

case ARGV[0]
when 'header'
  ERB.new(win32_h_template, trim_mode: '-').run
when 'impl'
  ERB.new(win32_c_template, trim_mode: '-').run
else
  puts 'Specify header or impl.'
  exit
end
