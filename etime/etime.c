#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

int main(int argc, const char ** argv)
{
   LARGE_INTEGER frequency = {0};
   QueryPerformanceFrequency(&frequency);

   char buffer[4096] = {0};
   for (size_t i = 1; i < argc; ++i)
   {
      strcat_s(buffer, 4096, argv[i]);
      strcat_s(buffer, 4096, " ");
   }

   STARTUPINFO si = {.cb = sizeof(STARTUPINFO)};
   PROCESS_INFORMATION pi = {0};

   LARGE_INTEGER start_time = {0};
   QueryPerformanceCounter(&start_time);

   if (CreateProcessA(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0)
   {
      fprintf(stderr, "error: couldn't execute command\n");
      exit(1);
   }
   WaitForSingleObject(pi.hProcess, INFINITE);

   LARGE_INTEGER end_time = {0};
   QueryPerformanceCounter(&end_time);
   const double seconds = (double)(end_time.QuadPart - start_time.QuadPart) * 1000000.0 / (double)frequency.QuadPart / 1000000.0;
   printf("Command took %.6f seconds\n", seconds);
}
