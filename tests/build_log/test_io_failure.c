/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_log/test_io_failure.c
 * PURPOSE: Inject native I/O failures to check retained prefixes and truthful close status.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
static size_t remaining = SIZE_MAX;
static int fail_flush, fail_close, close_calls;
#ifdef _WIN32
/* Use real temporary files, but deterministically fail the native boundary.
 * No disk filling, volume changes or production fault hooks are needed. */
static BOOL FixtureWrite(HANDLE file, LPCVOID bytes, DWORD length, LPDWORD written, LPOVERLAPPED overlap)
{
    if (remaining == 0U) { *written = 0U; SetLastError(ERROR_DISK_FULL); return FALSE; }
    if ((size_t)length > remaining) length = (DWORD)remaining;
    BOOL result = WriteFile(file, bytes, length, written, overlap);
    if (remaining != SIZE_MAX) remaining -= (size_t)*written;
    return result;
}
static BOOL FixtureFlush(HANDLE file)
{
    if (fail_flush) { SetLastError(ERROR_WRITE_FAULT); return FALSE; }
    return FlushFileBuffers(file);
}
static BOOL FixtureClose(HANDLE file)
{
    ++close_calls; BOOL result = CloseHandle(file); return fail_close ? FALSE : result;
}
#define WriteFile FixtureWrite
#define FlushFileBuffers FixtureFlush
#define CloseHandle FixtureClose
#else
static ssize_t FixtureWrite(int file, const void *bytes, size_t length)
{
    if (remaining == 0U) { errno = ENOSPC; return -1; }
    if (length > remaining) length = remaining;
    ssize_t result = write(file, bytes, length);
    if (remaining != SIZE_MAX && result > 0) remaining -= (size_t)result;
    return result;
}
static int FixtureFlush(int file)
{
    if (fail_flush) { errno = EIO; return -1; }
    return fsync(file);
}
static int FixtureClose(int file)
{
    ++close_calls; int result = close(file); return fail_close ? -1 : result;
}
#define write FixtureWrite
#define fsync FixtureFlush
#define close FixtureClose
#endif
/* Compile the actual writer with only its OS I/O boundary replaced. This test
 * target does not link the normal writer object or alter its shipped behaviour. */
#include "../../src/platform/output_file.c"

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY]; FixtureDirectory(root); FixturePath(path, root, "partial.log");
    UmiOutputFile *file = NULL; UmiOutputFileSnapshot snapshot;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    if (strcmp(argv[1], "partial") == 0) remaining = 3U;
    UmiStatus status = UmiOutputFileWrite(file, "abcdefgh", 8U);
    if (remaining == 0U) {
        CHECK(status == UMI_STATUS_IO_ERROR);
        remaining = SIZE_MAX;
        CHECK(UmiOutputFileWrite(file, "retry", 5U) == UMI_STATUS_IO_ERROR);
    } else CHECK(status == UMI_STATUS_OK);
    fail_flush = strcmp(argv[1], "flush") == 0; fail_close = strcmp(argv[1], "close") == 0;
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_IO_ERROR);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_IO_ERROR && close_calls == 1);
    CHECK(UmiOutputFileRead(file, &snapshot) == UMI_STATUS_OK && snapshot.closed && snapshot.status == UMI_STATUS_IO_ERROR);
    UmiOutputFileDestroy(file); CHECK(close_calls == 1);
    size_t length; unsigned char *bytes = FixtureRead(path, &length);
    CHECK(length == snapshot.bytes_written && length == (strcmp(argv[1], "partial") == 0 ? 3U : 8U));
    CHECK(memcmp(bytes, "abcdefgh", length) == 0); free(bytes); return 0;
}
