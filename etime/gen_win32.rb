require 'erb'

aliases = {
  BOOL: 'int',
  DWORD: 'uint32',
  FARPROC: 'void*',
  HANDLE: 'void*',
  HMODULE: 'void*',
  LPBYTE: 'unsigned char*',
  LPCSTR: 'const char*',
  LPPROCESS_INFORMATION: 'WIN32_PROCESS_INFORMATION*',
  LPSTARTUPINFOA: 'WIN32_STARTUPINFOA*',
  LPSTR: 'char*',
  LPVOID: 'void*',
  WORD: 'uint16',
}

defines = {
  INFINITE: 0xffffffff,
}

types = <<~EOF
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
  ['kernel32', 'BOOL', 'CreateProcessA', 'LPCSTR lpApplicationName, LPSTR lpCommandLine, void* lpProcessAttributes, void* lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation'],
  ['kernel32', 'BOOL', 'QueryPerformanceCounter', 'int64* lpPerformanceCount'],
  ['kernel32', 'BOOL', 'QueryPerformanceFrequency', 'int64* lpFrequency'],
  ['kernel32', 'DWORD', 'WaitForSingleObject', 'HANDLE hHandle, DWORD dwMilliseconds'],
]

win32_h_template = <<~EOF
  #ifndef WIN32_H
  #define WIN32_H

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

  typedef struct Win32Fns
  {
  <% functions.each do |func| -%>
      Win32_<%= func[2] %> <%= func[2] %>;
  <% end -%>
  } Win32Fns;

  void Win32_LoadFunctions(Win32Fns* win32);

  <% aliases.each_key do |key| -%>
  #undef <%= key %>
  <% end -%>

  #endif /* WIN32_H */
EOF

win32_c_template = <<~EOF
  #include "win32.h"

  void Win32_LoadFunctions(Win32Fns* win32)
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
