/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_channel_recording/test_finish.c
 *
 * PURPOSE:
 *   Inject final recording flush failures without changing shipped native file behavior.
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
#endif
#include "../build_log/fixture.h"
#include "../../src/platform/process_channel_internal.h"
static int failFlush, failClose, closeCalls;
#ifdef _WIN32
static BOOL TestFlush(HANDLE file)
{
    if (failFlush) { SetLastError(ERROR_WRITE_FAULT); return FALSE; }
    return FlushFileBuffers(file);
}
static BOOL TestClose(HANDLE file)
{
    ++closeCalls;
    BOOL result = CloseHandle(file);
    return failClose ? FALSE : result;
}
#define FlushFileBuffers TestFlush
#define CloseHandle TestClose
#else
static int TestFlush(int file)
{
    if (failFlush) { errno = EIO; return -1; }
    return fsync(file);
}
static int TestClose(int file)
{
    ++closeCalls;
    int result = close(file);
    return failClose ? -1 : result;
}
#define fsync TestFlush
#define close TestClose
#endif
/* Use the actual finalization and writer implementations with only their native
 * flush/close boundary replaced. No production fault hook or full disk is needed. */
#include "../../src/platform/output_file.c"
#include "../../src/platform/process_channel.c"
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "success") != 0 && strcmp(argv[1], "flush") != 0 &&
        strcmp(argv[1], "close") != 0) return 2;
    UmiProcessChannel channel = {0};
    CHECK(PcFinishDiagnostics(&channel) == UMI_STATUS_OK);
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "errors.log");
    CHECK(UmiOutputFileCreate(path, &channel.diagnosticFile) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(channel.diagnosticFile, "evidence", 8U) == UMI_STATUS_OK);
    channel.snapshot.running = 1;
    CHECK(PcFinishDiagnostics(&channel) == UMI_STATUS_OK && closeCalls == 0);
    failFlush = strcmp(argv[1], "flush") == 0;
    failClose = strcmp(argv[1], "close") == 0;
    channel.snapshot.running = 0;
    UmiStatus expected = failFlush || failClose ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
    CHECK(PcFinishDiagnostics(&channel) == expected && closeCalls == 1);
    CHECK(PcFinishDiagnostics(&channel) == expected && closeCalls == 1);
    UmiOutputFileSnapshot snapshot;
    CHECK(UmiOutputFileRead(channel.diagnosticFile, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.closed && snapshot.status == expected);
    UmiOutputFileDestroy(channel.diagnosticFile);
    CHECK(closeCalls == 1);
    size_t size; unsigned char *bytes = FixtureRead(path, &size);
    CHECK(size == 8U && memcmp(bytes, "evidence", size) == 0); free(bytes);
    return 0;
}
