/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_input_cleanup_status.c
 *
 * PURPOSE:
 *   Verify bounded input keeps the first failure while always releasing native handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#if !defined(_WIN32) && !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#endif
#include "umicom/platform/input_file.h"
#include "umicom/platform/input_stream.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static int fail_close, fail_allocation, change_metadata, fail_read;
static size_t closes, metadata_calls, callback_calls, callback_bytes;
static UmiStatus callback_status = UMI_STATUS_OK;
static void *InputAllocate(size_t bytes)
{
    return fail_allocation ? NULL : malloc(bytes);
}
#ifdef _WIN32
static BOOL WINAPI InputClose(HANDLE file)
{
    ++closes;
    BOOL closed = CloseHandle(file);
    return closed && !fail_close;
}
static BOOL WINAPI InputMetadata(HANDLE file, LPBY_HANDLE_FILE_INFORMATION info)
{
    BOOL read = GetFileInformationByHandle(file, info);
    ++metadata_calls;
    if (read && change_metadata && metadata_calls == 2U)
        info->ftLastWriteTime.dwLowDateTime ^= 1U;
    return read;
}
static BOOL WINAPI InputRead(HANDLE file, LPVOID bytes, DWORD capacity,
    LPDWORD received, LPOVERLAPPED overlapped)
{
    if (fail_read) { *received = 0U; return FALSE; }
    return ReadFile(file, bytes, capacity, received, overlapped);
}
#define CloseHandle InputClose
#define GetFileInformationByHandle InputMetadata
#define ReadFile InputRead
#else
static int InputClose(int file)
{
    ++closes;
    int closed = close(file);
    return closed == 0 && fail_close ? -1 : closed;
}
static int InputMetadata(int file, struct stat *info)
{
    int read = fstat(file, info);
    ++metadata_calls;
    if (read == 0 && change_metadata && metadata_calls == 2U) {
#if defined(__APPLE__)
        info->st_mtimespec.tv_nsec ^= 1L;
#else
        info->st_mtim.tv_nsec ^= 1L;
#endif
    }
    return read;
}
static ssize_t InputRead(int file, void *bytes, size_t capacity)
{
    if (fail_read) { errno = EIO; return -1; }
    return read(file, bytes, capacity);
}
#define close InputClose
#define fstat InputMetadata
#define read InputRead
#endif
/* The hooks close the actual handle and then report controlled failures.
 * Metadata changes are simulated on the returned observation, never by
 * altering the fixture file. Production readers and path validation stay real. */
#define malloc InputAllocate
#include "../src/platform/input_file.c"
#include "../src/platform/input_stream.c"
#undef malloc
#ifdef _WIN32
#undef CloseHandle
#undef GetFileInformationByHandle
#undef ReadFile
#else
#undef close
#undef fstat
#undef read
#endif
static UmiStatus Observe(const unsigned char *bytes, size_t size, void *context)
{
    (void)context;
    ++callback_calls;
    callback_bytes += size;
    if (size != 16U || memcmp(bytes, "0123456789ABCDEF", 16U) != 0)
        return UMI_STATUS_INVALID_STATE;
    return callback_status;
}
int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    const char *name = argv[1];
    static const char *cases[] = {
        "file-success",
        "file-close",
        "file-capacity",
        "file-capacity-close",
        "file-allocation",
        "file-allocation-close",
        "file-changed",
        "file-changed-close",
        "file-read",
        "file-read-close",
        "file-empty",
        "file-empty-close",
        "stream-success",
        "stream-close",
        "stream-capacity",
        "stream-capacity-close",
        "stream-changed",
        "stream-changed-close",
        "stream-read",
        "stream-read-close",
        "stream-cancel",
        "stream-cancel-close",
        "stream-callback-oom-close",
        "stream-callback-capacity-close",
        "stream-callback-invalid-close",
        "stream-empty",
        "stream-empty-close"
    };
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(name, cases[i])) known = 1;
    if (!known) return 2;
    int streaming = strncmp(name, "stream-", 7U) == 0;
    int empty = strstr(name, "empty") != NULL;
    int limited = strstr(name, "capacity") != NULL && strstr(name, "callback") == NULL;
    fail_close = strstr(name, "-close") != NULL;
    fail_allocation = strstr(name, "allocation") != NULL;
    change_metadata = strstr(name, "changed") != NULL;
    fail_read = strstr(name, "-read") != NULL;
    if (strstr(name, "cancel") != NULL) callback_status = UMI_STATUS_CANCELLED;
    if (strstr(name, "callback-oom") != NULL) callback_status = UMI_STATUS_OUT_OF_MEMORY;
    if (strstr(name, "callback-capacity") != NULL) callback_status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strstr(name, "callback-invalid") != NULL) callback_status = UMI_STATUS_INVALID_STATE;
    const char *path = empty ? argv[3] : argv[2];
    size_t limit = limited ? 15U : 16U;
    UmiStatus expected = limited ? UMI_STATUS_CAPACITY_EXCEEDED :
        (fail_allocation ? UMI_STATUS_OUT_OF_MEMORY :
        (fail_read ? UMI_STATUS_IO_ERROR :
        (callback_status != UMI_STATUS_OK ? callback_status :
        (change_metadata ? UMI_STATUS_BUSY :
        (fail_close ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK)))));
    UmiStatus status;
    if (streaming) {
        uint64_t size = 99U;
        status = UmiInputFileVisit(path, limit, Observe, NULL, &size);
        CHECK(status == expected);
        CHECK(size == (expected == UMI_STATUS_OK && !empty ? 16U : 0U));
        size_t expected_calls = limited || fail_read || empty ? 0U : 1U;
        CHECK(callback_calls == expected_calls && callback_bytes == expected_calls * 16U);
    } else {
        unsigned char *bytes = (unsigned char *)(uintptr_t)1U;
        size_t size = 99U;
        status = UmiInputFileRead(path, limit, &bytes, &size);
        CHECK(status == expected);
        if (status == UMI_STATUS_OK) {
            CHECK(bytes != NULL && size == (empty ? 0U : 16U));
            CHECK(bytes[size] == 0U);
            if (!empty) CHECK(memcmp(bytes, "0123456789ABCDEF", 16U) == 0);
            UmiInputFileFree(bytes);
        } else CHECK(bytes == NULL && size == 0U);
    }
    CHECK(closes == 1U);
    return 0;
}
