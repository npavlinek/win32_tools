#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fixed_width_integers.h"
#include "win32.h"

#define MAXIMUM(a, b) (a) > (b) ? (a) : (b)

static int is_watching = 1;

static void* Alloc(size_t size)
{
    void* mem = calloc(1, size);
    if (mem == NULL)
    {
        fprintf(stderr, "error: couldn't allocate memory\n");
        abort();
    }
    return mem;
}

static void* Realloc(void* ptr, size_t size)
{
    void* mem = realloc(ptr, size);
    if (mem == NULL)
    {
        fprintf(stderr, "error: couldn't reallocate memory");
        abort();
    }
    return mem;
}

typedef struct stringbuf_s
{
    char* buf;
    size_t used;
    size_t size;
} stringbuf_t;

static void CreateStringBuffer(stringbuf_t* buf)
{
    buf->size = 4096;
    buf->buf = Alloc(buf->size);
    buf->used = 0;
}

static void ResetStringBuffer(stringbuf_t* buf)
{
    buf->used = 0;
}

static void PushChar(stringbuf_t* buf, char ch)
{
    if ((buf->size - buf->used - 1) == 0)
    {
        buf->size += buf->size;
        buf->buf = Realloc(buf->buf, buf->size);
    }
    buf->buf[buf->used++] = ch;
    buf->buf[buf->used] = '\0';
}

static void PushString(stringbuf_t* buf, const char* str)
{
    const size_t len = strlen(str);
    if (len >= (buf->size - buf->used - 1))
    {
        buf->size += MAXIMUM((len + 1) * sizeof(char), buf->size);
        buf->buf = Realloc(buf->buf, buf->size);
    }
    memcpy(&buf->buf[buf->used], str, len);
    buf->used += len * sizeof(char);
    buf->buf[buf->used] = '\0';
}

static void PushUTF8FromUTF16(stringbuf_t* buf, const wchar_t* str)
{
    size_t i;
    for (i = 0; str[i] != L'\0'; ++i)
    {
        const unsigned int unit = str[i];
        if (unit <= 0xd7ff || (unit >= 0xe000 && unit <= 0xffff))
        {
            /* Characters in this range fall into the Basic Multilingual Plane (BMP), which contains
             * most commonly used characters. The encoding scheme is simple: UTF-16 code units are
             * numerically equal to Unicode code points. */

            if (unit <= 0x7f)
                PushChar(buf, (char)(unit & 0xff));
            else if (unit >= 0x80 && unit <= 0x7ff)
            {
                PushChar(buf, (char)((0x6 << 5) | (unit >> 6)));
                PushChar(buf, (char)((0x2 << 6) | (unit & 0x3f)));
            }
            else if (unit >= 0x800 && unit <= 0xffff)
            {
                PushChar(buf, (char)((0xe << 4) | (unit >> 12)));
                PushChar(buf, (char)((0x2 << 6) | (unit >> 6)));
                PushChar(buf, (char)((0x2 << 6) | (unit & 0x3f)));
            }
            else if (unit >= 0x10000 && unit <= 0x10ffff)
            {
                PushChar(buf, (char)((0x1e << 3) | (unit >> 18)));
                PushChar(buf, (char)((0x2 << 6) | (unit >> 12)));
                PushChar(buf, (char)((0x2 << 6) | (unit >> 6)));
                PushChar(buf, (char)((0x2 << 6) | (unit & 0x3f)));
            }
            else
                assert(0);
        }
        else if (unit >= 0xd800 && unit <= 0xdbff)
        {
            /* Characters in this range are specifically reserved for UTF-16 surrogate pairs, i.e.
             * Unicode code points that get encoded with two UTF-16 code units. The encoding scheme
             * is also rather simple:
             *
             * 1. 0x10000 is subtracted from the Unicode code point.
             *
             * 2. The high 10 bits of the resulting 20-bit number are added with 0xd800 to produce a
             * 16-bit number of the following form: 110110xxxxxxxxxx. This is the first UTF-16 code
             * unit.
             *
             * 3. The low 10 bits of the resulting 20-bit number added with 0xdc00 to produce a
             * 16-bit number of the following form: 110111xxxxxxxxxx. This is the second UTF-16 code
             * unit.
             *
             * To decode we just do the reverse: extract the bottom 10 bits from each UTF-16 unit
             * and OR them together, then add 0x10000. */

            const unsigned int unit0 = unit & 0x3ff;
            const unsigned int unit1 = str[++i] & 0x3ff;
            const unsigned int unicode = ((unit0 << 10) | unit1) + 0x10000;

            assert((str[i - 1] & 0xd800) == 0xd800 && (str[i] & 0xdc00) == 0xdc00);

            PushChar(buf, (char)((0x1e << 3) | (unicode >> 18)));
            PushChar(buf, (char)((0x2 << 6) | (unicode >> 12)));
            PushChar(buf, (char)((0x2 << 6) | (unicode >> 6)));
            PushChar(buf, (char)((0x2 << 6) | (unicode & 0x3f)));
        }
    }
}

static int CtrlHandler(uint32 type)
{
    switch (type)
    {
    case WIN32_CTRL_C_EVENT:
        is_watching = 0;
        return 1;
    default:
        return 0;
    }
}

int main(int argc, const char** argv)
{
    win32_t win32 = { 0 };
    stringbuf_t sbuf;
    char* cmd = NULL;
    char* dbuf = NULL;
    uint32 dbuf_size;
    void* dir_handle = NULL;
    int i;

    if (argc < 2)
    {
        fprintf(stderr, "error: no command given\n");
        return 1;
    }

    LoadWin32Functions(&win32);
    win32.SetConsoleCtrlHandler(CtrlHandler, 1);

    CreateStringBuffer(&sbuf);
    for (i = 1; i < argc; ++i)
    {
        PushString(&sbuf, argv[i]);
        PushString(&sbuf, " ");
    }
    cmd = Alloc(sbuf.used + sizeof(char));
    memcpy(cmd, sbuf.buf, sbuf.used);

    dbuf_size = win32.GetCurrentDirectoryA(0, NULL);
    dbuf = Alloc((dbuf_size + 1) * sizeof(char));
    win32.GetCurrentDirectoryA(dbuf_size, dbuf);

    dir_handle = win32.CreateFileA(dbuf, WIN32_FILE_LIST_DIRECTORY,
        WIN32_FILE_SHARE_READ | WIN32_FILE_SHARE_WRITE, NULL, WIN32_OPEN_EXISTING,
        WIN32_FILE_FLAG_BACKUP_SEMANTICS | WIN32_FILE_FLAG_OVERLAPPED, NULL);
    if ((intptr)dir_handle == WIN32_INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "error: couldn't open directory '%s'\n", dbuf);
        return 1;
    }

    while (is_watching)
    {
        char info[1024] = { 0 };
        WIN32_OVERLAPPED overlapped = { 0 };
        uint32 bytes = 0;
        WIN32_FILE_NOTIFY_INFORMATION* file_info = NULL;
        WIN32_STARTUPINFOA si = { 0 };
        WIN32_PROCESS_INFORMATION pi = { 0 };

        win32.ReadDirectoryChangesW(dir_handle, info, 1024, 1,
            WIN32_FILE_NOTIFY_CHANGE_SIZE | WIN32_FILE_NOTIFY_CHANGE_LAST_WRITE, NULL,
            &overlapped, NULL);

        if (!win32.GetOverlappedResultEx(dir_handle, &overlapped, &bytes, 100, 0))
        {
            const uint32 err = win32.GetLastError();
            switch (err)
            {
            case WIN32_WAIT_TIMEOUT:
                continue;
            default:
                fprintf(stderr, "error: %u\n", err);
                break;
            }
        }

        file_info = (WIN32_FILE_NOTIFY_INFORMATION*)&info;
        ResetStringBuffer(&sbuf);
        PushUTF8FromUTF16(&sbuf, file_info->FileName);

        if (sbuf.buf[0] == '.')
            continue;
        printf(">>> Detected changes: %s\n", sbuf.buf);

        si.cb = sizeof(WIN32_STARTUPINFOA);
        win32.CreateProcessA(NULL, cmd, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
        win32.WaitForSingleObject(pi.hProcess, WIN32_INFINITE);
        printf(">>> Done\n");
    }

    win32.CloseHandle(dir_handle);
    return 0;
}
