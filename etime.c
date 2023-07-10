#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct str {
    char *data;
    size_t size;
    size_t cap;
} str;

static str strNew(size_t cap) {
    str str = {0};
    void *data = malloc((cap + 1) * sizeof(char));
    if (data == NULL) return str;
    str.data = data;
    str.data[0] = '\0';
    str.size = 0;
    str.cap = cap;
    return str;
}

static void strFree(str *str) {
    if (str == NULL) return;
    free(str->data);
    memset(str, 0, sizeof(*str));
}

static int strAppend(str *str, const char *s) {
    size_t len = strlen(s);
    if (str == NULL) return 0;
    if ((str->cap - str->size) < len) {
        size_t new_cap = str->size + len;
        char *data = realloc(str->data, (new_cap + 1) * sizeof(char));
        if (data == NULL) return 0;
        str->data = data;
        str->cap = new_cap;
    }
    memcpy(&str->data[str->size], s, len);
    str->size += len;
    str->data[str->size] = '\0';
    return 1;
}

int main(int argc, const char **argv) {
    LARGE_INTEGER freq, begin, end;
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    str cmd;
    int i;
    double elapsed;

    if (argc < 2) {
        fprintf(stderr, "error: no command to execute\n");
        exit(EXIT_FAILURE);
    }

    QueryPerformanceFrequency(&freq);

    cmd = strNew(1024);
    if (cmd.data == NULL) goto nomem;

    for (i = 1; i < argc; ++i) {
        if (!strAppend(&cmd, argv[i])) goto nomem;
        if (i != (argc - 1)) strAppend(&cmd, " ");
    }

    QueryPerformanceCounter(&begin);

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(STARTUPINFOA);
    if (CreateProcessA(NULL, cmd.data, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == 0) {
        fprintf(stderr, "error: failed to execute command '%s'\n", cmd.data);
        strFree(&cmd);
        exit(EXIT_FAILURE);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    QueryPerformanceCounter(&end);

    elapsed = (double)(end.QuadPart - begin.QuadPart);
    elapsed *= 1000.0 * 1000.0 * 1000.0;
    elapsed /= (double)freq.QuadPart;

    if (elapsed >= (1000.0 * 1000.0 * 1000.0))
        printf("Command '%s' took %.2f s\n", cmd.data, elapsed / (1000.0 * 1000.0 * 1000.0));
    else if (elapsed >= (1000.0 * 1000.0))
        printf("Command '%s' took %.2f ms\n", cmd.data, elapsed / (1000.0 * 1000.0));
    else if (elapsed >= 1000.0)
        printf("Command '%s' took %.2f us\n", cmd.data, elapsed / 1000.0);
    else
        printf("Command '%s' took %.2f ns\n", cmd.data, elapsed);

    strFree(&cmd);
    return 0;

nomem:
    fprintf(stderr, "error: not enough memory available to store command line\n");
    strFree(&cmd);
    exit(EXIT_FAILURE);
}
