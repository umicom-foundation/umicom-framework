/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_rooted_create_failures.c
 *
 * PURPOSE:
 *   Verify exclusive-write progress and close ownership when native I/O fails.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#endif
#include "umicom/platform/rooted_files.h"
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static const char *mode;
static unsigned opened, inspected, writes, flushes, closes;
static size_t accepted;
static int contract_error;

/* These narrow native-operation substitutions inject errors into the actual
 * implementation include. They never touch disk. The companion filesystem
 * test covers parent confinement and actual creation against real storage. */
#ifdef _WIN32
typedef struct RootedParent { wchar_t path[32]; } RootedParent;
static UmiStatus RootedError(DWORD error)
{
    return error == ERROR_FILE_EXISTS ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
}
static HANDLE FakeCreate(LPCWSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES security,
    DWORD disposition, DWORD attributes, HANDLE template_file)
{
    ++opened;
    if (name == NULL || access != (GENERIC_WRITE | FILE_READ_ATTRIBUTES) || share != 0U || security != NULL ||
        disposition != CREATE_NEW || (attributes & FILE_FLAG_OPEN_REPARSE_POINT) == 0U ||
        template_file != NULL) contract_error = 1;
    if (strcmp(mode, "open") == 0) {
        SetLastError(ERROR_FILE_EXISTS);
        return INVALID_HANDLE_VALUE;
    }
    return (HANDLE)(uintptr_t)7U;
}
static UmiStatus RootedInspectHandle(HANDLE handle, BY_HANDLE_FILE_INFORMATION *info)
{
    ++inspected;
    (void)handle;
    memset(info, 0, sizeof(*info));
    return strcmp(mode, "inspect") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static BOOL FakeWrite(HANDLE handle, LPCVOID bytes, DWORD size, LPDWORD count, LPOVERLAPPED overlap)
{
    ++writes;
    if (handle != (HANDLE)(uintptr_t)7U || bytes == NULL || size == 0U || overlap != NULL) contract_error = 1;
    if (strcmp(mode, "write") == 0) { *count = 0U; return FALSE; }
    if (strcmp(mode, "zero") == 0) { *count = 0U; return TRUE; }
    if (strcmp(mode, "partial") == 0) { *count = size > 2U ? 2U : size; accepted += *count; return FALSE; }
    *count = strcmp(mode, "short") == 0 && size > 2U ? 2U : size;
    accepted += *count;
    return TRUE;
}
static BOOL FakeFlush(HANDLE handle)
{
    ++flushes;
    if (handle != (HANDLE)(uintptr_t)7U) contract_error = 1;
    return strcmp(mode, "flush") == 0 ? FALSE : TRUE;
}
static BOOL FakeClose(HANDLE handle)
{
    ++closes;
    if (handle != (HANDLE)(uintptr_t)7U) contract_error = 1;
    return strcmp(mode, "close") == 0 ? FALSE : TRUE;
}
#define CreateFileW FakeCreate
#define WriteFile FakeWrite
#define FlushFileBuffers FakeFlush
#define CloseHandle FakeClose
#include "../../src/platform/rooted_create_win32.inc"
#undef CreateFileW
#undef WriteFile
#undef FlushFileBuffers
#undef CloseHandle
#else
typedef struct RootedParent { int parent; const char *leaf; } RootedParent;
static UmiStatus RootedError(int error)
{
    return error == EEXIST ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_IO_ERROR;
}
static int FakeOpen(int parent, const char *leaf, int flags, mode_t permissions)
{
    ++opened;
    if (parent != 3 || strcmp(leaf, "example.c") != 0 ||
        flags != (O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC) || permissions != 0600)
        contract_error = 1;
    if (strcmp(mode, "open") == 0) { errno = EEXIST; return -1; }
    return 7;
}
static int FakeInspect(int file, struct stat *info)
{
    ++inspected;
    if (file != 7) contract_error = 1;
    memset(info, 0, sizeof(*info));
    info->st_mode = S_IFREG;
    return strcmp(mode, "inspect") == 0 ? -1 : 0;
}
static ssize_t FakeWrite(int file, const void *bytes, size_t size)
{
    ++writes;
    if (file != 7 || bytes == NULL || size == 0U) contract_error = 1;
    if (strcmp(mode, "interrupt-write") == 0 && writes == 1U) { errno = EINTR; return -1; }
    if (strcmp(mode, "write") == 0 || (strcmp(mode, "partial") == 0 && writes > 1U)) {
        errno = EIO; return -1;
    }
    if (strcmp(mode, "zero") == 0) return 0;
    size_t count = (strcmp(mode, "short") == 0 || strcmp(mode, "partial") == 0) && size > 2U ? 2U : size;
    accepted += count;
    return (ssize_t)count;
}
static int FakeFlush(int file)
{
    ++flushes;
    if (file != 7) contract_error = 1;
    if (strcmp(mode, "interrupt-flush") == 0 && flushes == 1U) { errno = EINTR; return -1; }
    if (strcmp(mode, "flush") == 0) { errno = EIO; return -1; }
    return 0;
}
static int FakeClose(int file)
{
    ++closes;
    if (file != 7) contract_error = 1;
    /* An interrupted close must not be repeated on a potentially reused fd. */
    if (strcmp(mode, "close") == 0) { errno = EINTR; return -1; }
    return 0;
}
#define openat FakeOpen
#define fstat FakeInspect
#define write FakeWrite
#define fsync FakeFlush
#define close FakeClose
#include "../../src/platform/rooted_create_posix.inc"
#undef openat
#undef fstat
#undef write
#undef fsync
#undef close
#endif

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    mode = argv[1];
    static const char *const cases[] = {"open", "inspect", "write", "partial", "zero",
        "flush", "close", "short", "success", "empty"
#ifndef _WIN32
        , "interrupt-write", "interrupt-flush"
#endif
    };
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0) known = 1;
    if (!known) return 2;
#ifdef _WIN32
    RootedParent parent = {L"example.c"};
#else
    RootedParent parent = {3, "example.c"};
#endif
    UmiRootedFileCreation report = {0};
    const char bytes[] = "verified example";
    size_t size = strcmp(mode, "empty") == 0 ? 0U : sizeof(bytes) - 1U;
    UmiStatus status = RootedCreateNew(&parent, size == 0U ? NULL : bytes, size, &report);
    CHECK(opened == 1U && !contract_error);
    if (strcmp(mode, "open") == 0) {
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && !report.created && !report.complete);
        CHECK(report.bytes_written == 0U && inspected == 0U && writes == 0U && flushes == 0U && closes == 0U);
        return 0;
    }
    CHECK(report.created && inspected == 1U && closes == 1U && report.bytes_written == accepted);
    int failed = strcmp(mode, "inspect") == 0 || strcmp(mode, "write") == 0 ||
        strcmp(mode, "partial") == 0 || strcmp(mode, "zero") == 0 ||
        strcmp(mode, "flush") == 0 || strcmp(mode, "close") == 0;
    CHECK(status == (failed ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK));
    CHECK(report.complete == !failed);
    if (strcmp(mode, "inspect") == 0) CHECK(writes == 0U && flushes == 0U);
    if (strcmp(mode, "write") == 0 || strcmp(mode, "partial") == 0 || strcmp(mode, "zero") == 0)
        CHECK(flushes == 0U);
    if (strcmp(mode, "partial") == 0) CHECK(report.bytes_written == 2U);
    if (strcmp(mode, "short") == 0) CHECK(writes > 1U);
    if (strcmp(mode, "empty") == 0) CHECK(writes == 0U);
    if (!failed) CHECK(report.bytes_written == size);
#ifndef _WIN32
    if (strcmp(mode, "interrupt-write") == 0) CHECK(writes == 2U);
    if (strcmp(mode, "interrupt-flush") == 0) CHECK(flushes == 2U);
#endif
    return 0;
}
