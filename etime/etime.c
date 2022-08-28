#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int main(int argc, const char **argv)
{
  LARGE_INTEGER frequency, start_time, end_time;
  STARTUPINFO si = {0};
  PROCESS_INFORMATION pi = {0};
  char buffer[4096] = {0};
  int i;
  double seconds;

  QueryPerformanceFrequency(&frequency);

  for (i = 1; i < argc; ++i) {
    strcat_s(buffer, 4096, argv[i]);
    strcat_s(buffer, 4096, " ");
  }

  QueryPerformanceCounter(&start_time);

  si.cb = sizeof(STARTUPINFO);
  if (CreateProcessA(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0) {
    fprintf(stderr, "error: couldn't execute command\n");
    exit(1);
  }
  WaitForSingleObject(pi.hProcess, INFINITE);

  QueryPerformanceCounter(&end_time);
  seconds = (double)(end_time.QuadPart - start_time.QuadPart) / (double)frequency.QuadPart;
  printf("Command took %.6f seconds\n", seconds);
}
