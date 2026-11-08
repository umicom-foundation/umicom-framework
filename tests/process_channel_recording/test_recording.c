/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_channel_recording/test_recording.c
 *
 * PURPOSE:
 *   Verify recorded native child output, exclusive paths and unchanged protocol streams.
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
#include "umicom/platform/process_channel.h"
#ifndef _WIN32
#include <fcntl.h>
#include <time.h>
#endif
static void Pause(void)
{
#ifdef _WIN32
    Sleep(10U);
#else
    struct timespec delay = {0, 10000000};
    while (nanosleep(&delay, &delay) != 0 && errno == EINTR) {}
#endif
}
static long FileSize(const char *path)
{
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide,
                            (int)UMI_PATH_CAPACITY) != 0);
    FILE *file = _wfopen(wide, L"rb");
#else
    FILE *file = fopen(path, "rb");
#endif
    if (file == NULL) return -1;
    long size = fseek(file, 0L, SEEK_END) == 0 ? ftell(file) : -1;
    CHECK(fclose(file) == 0);
    return size;
}
static void RenameAfterCompletion(const char *source, const char *destination)
{
#ifdef _WIN32
    wchar_t from[UMI_PATH_CAPACITY], to[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, source, -1, from,
                            (int)UMI_PATH_CAPACITY) != 0);
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, destination, -1, to,
                            (int)UMI_PATH_CAPACITY) != 0);
    /* The writer excludes delete sharing while open. This also verifies that
     * the checked completion path released its Windows file owner. */
    CHECK(MoveFileW(from, to));
#else
    CHECK(rename(source, destination) == 0);
#endif
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *mode = argv[1];
    const char *known[] = {"binary", "large", "unicode", "existing", "relative", "empty",
        "null-path", "missing-parent", "invalid-request", "null-output", "terminate",
        "destroy", "exit", "closed-stdio", "borrowed-path", "legacy"};
    int valid = 0;
    for (size_t index = 0; index < sizeof known / sizeof known[0]; ++index)
        if (strcmp(mode, known[index]) == 0) valid = 1;
    if (!valid) return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], retained[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9.log" : "errors.log");
    strcpy(retained, path);
    const char *childMode = strcmp(mode, "large") == 0 ? "large" :
        (strcmp(mode, "terminate") == 0 || strcmp(mode, "destroy") == 0) ? "wait" :
        strcmp(mode, "exit") == 0 ? "exit" : "binary";
    UmiProcessChannelRequest request = {argv[2], &childMode, 1U, root};
    UmiProcessChannel *channel = NULL;
    if (strcmp(mode, "existing") == 0) {
        UmiOutputFile *file = NULL;
        CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, "keep", 4U) == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
        CHECK(UmiProcessChannelOpenRecorded(&request, path, &channel) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(channel == NULL);
        size_t count; unsigned char *bytes = FixtureRead(path, &count);
        CHECK(count == 4U && memcmp(bytes, "keep", 4U) == 0); free(bytes); return 0;
    }
    if (strcmp(mode, "relative") == 0 || strcmp(mode, "empty") == 0 || strcmp(mode, "null-path") == 0) {
        const char *invalid = strcmp(mode, "relative") == 0 ? "relative.log" :
            strcmp(mode, "empty") == 0 ? "" : NULL;
        CHECK(UmiProcessChannelOpenRecorded(&request, invalid, &channel) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(channel == NULL && FileSize(path) == -1); return 0;
    }
    if (strcmp(mode, "missing-parent") == 0) {
        FixturePath(path, root, "missing/errors.log");
        CHECK(UmiProcessChannelOpenRecorded(&request, path, &channel) == UMI_STATUS_NOT_FOUND);
        CHECK(channel == NULL); return 0;
    }
    if (strcmp(mode, "invalid-request") == 0) {
        CHECK(UmiProcessChannelOpenRecorded(NULL, path, &channel) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(channel == NULL && FileSize(path) == -1); return 0;
    }
    if (strcmp(mode, "null-output") == 0) {
        CHECK(UmiProcessChannelOpenRecorded(&request, path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(FileSize(path) == -1); return 0;
    }
    UmiStatus status;
    if (strcmp(mode, "closed-stdio") == 0) {
#ifdef _WIN32
        fprintf(stderr, "SKIP: descriptor numbers below three are a POSIX scenario.\n");
        return 77;
#else
        int saved[3];
        for (int index = 0; index < 3; ++index) {
            saved[index] = fcntl(index, F_DUPFD_CLOEXEC, 3);
            CHECK(saved[index] >= 3);
        }
        for (int index = 0; index < 3; ++index) CHECK(close(index) == 0);
        status = UmiProcessChannelOpenRecorded(&request, path, &channel);
        for (int index = 0; index < 3; ++index) {
            CHECK(dup2(saved[index], index) == index); CHECK(close(saved[index]) == 0);
        }
#endif
    } else status = strcmp(mode, "legacy") == 0 ? UmiProcessChannelOpen(&request, &channel)
        : UmiProcessChannelOpenRecorded(&request, path, &channel);
    CHECK(status == UMI_STATUS_OK && channel != NULL);
    if (strcmp(mode, "borrowed-path") == 0) memset(path, 'x', strlen(path));
    if (strcmp(mode, "large") == 0) {
        /* Poll only the file, never the channel, while the child fills stderr.
         * A bounded stderr pipe would stall before reaching this byte count. */
        for (unsigned attempt = 0; attempt < 1000U && FileSize(retained) < 524288L; ++attempt) Pause();
        CHECK(FileSize(retained) == 524288L);
    }
    char output[128] = {0}; size_t used = 0;
    UmiProcessChannelSnapshot snapshot = {0};
    for (unsigned attempt = 0; attempt < 1000U && used < 14U; ++attempt) {
        size_t count = 0;
        status = UmiProcessChannelRead(channel, output + used, sizeof output - used, &count, 10U);
        CHECK(status == UMI_STATUS_OK || status == UMI_STATUS_TIMEOUT);
        used += count;
    }
    CHECK(used == 14U && memcmp(output, "protocol-only\n", 14U) == 0);
    if (strcmp(mode, "destroy") == 0) {
        UmiProcessChannelDestroy(channel); channel = NULL;
    } else {
        if (strcmp(mode, "terminate") == 0) {
            CHECK(UmiProcessChannelTerminate(channel) == UMI_STATUS_OK);
            CHECK(UmiProcessChannelTerminate(channel) == UMI_STATUS_OK);
        }
        for (unsigned attempt = 0; attempt < 1000U; ++attempt) {
            CHECK(UmiProcessChannelPoll(channel, &snapshot) == UMI_STATUS_OK);
            if (!snapshot.running) break;
            Pause();
        }
        CHECK(!snapshot.running);
        if (strcmp(mode, "terminate") == 0) CHECK(snapshot.terminated);
        else CHECK(snapshot.exitCode == (strcmp(mode, "exit") == 0 ? 23 : 0));
        CHECK(UmiProcessChannelPoll(channel, &snapshot) == UMI_STATUS_OK);
        if (strcmp(mode, "legacy") == 0) {
            CHECK(snapshot.diagnostics[0] == 'e' && snapshot.diagnostics[1] == '?');
            CHECK(FileSize(retained) == -1);
            UmiProcessChannelDestroy(channel); return 0;
        }
        CHECK(snapshot.diagnostics[0] == '\0' && !snapshot.diagnosticsTruncated);
        /* Rename before destruction proves checked completion closed its file. */
        FixturePath(path, root, "closed-recording.log");
        RenameAfterCompletion(retained, path); strcpy(retained, path);
        UmiProcessChannelDestroy(channel); channel = NULL;
    }
    size_t count; unsigned char *bytes = FixtureRead(retained, &count);
    if (strcmp(mode, "large") == 0) {
        CHECK(count == 524288U);
        for (size_t index = 0; index < count; ++index) CHECK(bytes[index] == (unsigned char)index);
    } else {
        const unsigned char expected[] = {'e', 0U, 0xffU, '\r', '\n', 'z'};
        CHECK(count == sizeof expected && memcmp(bytes, expected, count) == 0);
    }
    free(bytes); return 0;
}
