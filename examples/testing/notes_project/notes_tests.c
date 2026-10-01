/*-----------------------------------------------------------------------------
 * Umicom Notes testing example | Sammy Hegab | Umicom Foundation | MIT
 * Exercise a note's saved bytes using an automatically removed temporary file.
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <wchar.h>
#include <fcntl.h>
#include <io.h>
#else
#include <time.h>
#endif
/* Use the user's temporary directory on Windows. CREATE_NEW prevents an
 * existing file from being replaced, and the owned handle deletes on close. */
static FILE *NotesTemporaryFile(void)
{
#ifdef _WIN32
    wchar_t root[MAX_PATH + 1U], path[MAX_PATH + 128U];
    DWORD length = GetTempPathW(MAX_PATH + 1U, root);
    LARGE_INTEGER ticks;
    if (length == 0U || length > MAX_PATH || !QueryPerformanceCounter(&ticks)) {
        errno = EIO; return NULL;
    }
    for (unsigned attempt = 0U; attempt < 128U; ++attempt) {
        int written = swprintf(path, sizeof(path) / sizeof(path[0]),
            L"%lsumi-notes-%lu-%llu-%u.tmp", root, (unsigned long)GetCurrentProcessId(),
            (unsigned long long)ticks.QuadPart, attempt);
        if (written < 0 || (size_t)written >= sizeof(path) / sizeof(path[0])) break;
        HANDLE handle = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
            CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if (handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_EXISTS || GetLastError() == ERROR_ALREADY_EXISTS) continue;
            break;
        }
        int descriptor = _open_osfhandle((intptr_t)handle, _O_BINARY | _O_RDWR);
        if (descriptor == -1) { (void)CloseHandle(handle); return NULL; }
        FILE *file = _fdopen(descriptor, "w+b");
        if (file == NULL) (void)_close(descriptor);
        return file;
    }
    errno = EIO; return NULL;
#else
    return tmpfile();
#endif
}

static int CheckSavedNote(const char *title, const char *body)
{
    FILE *file = NotesTemporaryFile();
    if (file == NULL) { perror("Cannot create a temporary note"); return 1; }
    int failed = fprintf(file, "%s\n%s", title, body) < 0 || fflush(file) != 0;
    if (!failed && fseek(file, 0L, SEEK_SET) != 0) failed = 1;
    char expected[512], actual[512] = {0};
    int count = snprintf(expected, sizeof(expected), "%s\n%s", title, body);
    if (count < 0 || (size_t)count >= sizeof(expected)) failed = 1;
    if (!failed) {
        size_t bytes = fread(actual, 1U, sizeof(actual) - 1U, file);
        failed = ferror(file) || bytes != (size_t)count || memcmp(actual, expected, bytes) != 0;
    }
    if (fclose(file) != 0) failed = 1;
    return failed;
}
int main(int argc, char **argv)
{
    if (argc != 2) { (void)fprintf(stderr, "Choose save, unicode or index.\n"); return 2; }
    if (strcmp(argv[1], "save") == 0)
        return CheckSavedNote("Project meeting", "Build the Notes window and test saving.\n");
    if (strcmp(argv[1], "unicode") == 0)
        return CheckSavedNote("Research notes", "Symbols: \xCE\xBB and \xF0\x9F\x9A\x80\n");
    if (strcmp(argv[1], "index") == 0) {
        /* A bounded stand-in for a longer indexing test gives the Stop button
         * time to be exercised. No personal workspace or service is opened. */
        (void)puts("Indexing test started."); (void)fflush(stdout);
#ifdef _WIN32
        Sleep(5000U);
#else
        struct timespec delay = {5, 0};
        (void)nanosleep(&delay, NULL);
#endif
        return CheckSavedNote("Index entry", "notes.save\nnotes.unicode\n");
    }
    return 2;
}
