/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_local_replace_retry.c
 *
 * PURPOSE:
 *   Verify bounded Windows publication retries, permanent refusal and retained file contents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../build_log/fixture.h"
#include "umicom/platform/local_replace.h"
#include <wchar.h>

/* Compile the real replacement algorithm with only the operating-system
 * publication/wait boundary redirected. The native output writer still creates,
 * writes and flushes real isolated files. This checks retry decisions and byte
 * preservation without relying on a racing background process or real delays. */
static DWORD injected_error;
static unsigned failures_remaining, move_calls, wait_calls, remove_calls;
static bool refuse_cleanup, turn_destination_into_directory;
static wchar_t staging_path[UMI_PATH_CAPACITY], destination_path[UMI_PATH_CAPACITY];

static BOOL TestMove(LPCWSTR from, LPCWSTR to, DWORD flags)
{
    ++move_calls;
    CHECK(wcslen(from) < UMI_PATH_CAPACITY && wcslen(to) < UMI_PATH_CAPACITY);
    wcscpy(staging_path, from);
    wcscpy(destination_path, to);
    CHECK(flags == (MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH));
    if (failures_remaining != 0U)
    {
        --failures_remaining;
        SetLastError(injected_error);
        return FALSE;
    }
    return MoveFileExW(from, to, flags);
}

static void TestWait(DWORD milliseconds)
{
    ++wait_calls;
    CHECK(milliseconds == (DWORD)(wait_calls * 10U));
    if (turn_destination_into_directory)
    {
        CHECK(DeleteFileW(destination_path));
        CHECK(CreateDirectoryW(destination_path, NULL));
        turn_destination_into_directory = false;
    }
}

static BOOL TestRemove(LPCWSTR filename)
{
    ++remove_calls;
    CHECK(wcscmp(filename, staging_path) == 0);
    BOOL removed = refuse_cleanup ? FALSE : DeleteFileW(filename);
    /* Cleanup is allowed to change thread error state. The operation must have
     * copied its publication error before reaching this boundary. */
    SetLastError(ERROR_ACCESS_DENIED);
    return removed;
}

#define MoveFileExW TestMove
#define Sleep TestWait
#define DeleteFileW TestRemove
#define UmiLocalFileReplace UmiTestLocalFileReplace
#define UmiLocalFileReplaceDetailed UmiTestLocalFileReplaceDetailed
#include "../../src/platform/local_replace.c"
#undef UmiLocalFileReplaceDetailed
#undef UmiLocalFileReplace
#undef DeleteFileW
#undef Sleep
#undef MoveFileExW

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], filename[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(filename, root, "settings.bin");
    const unsigned char before[] = {1U, 0U, 3U}, after[] = {8U, 0U, 7U, 6U};
    CHECK(UmiLocalFileReplace(filename, before, sizeof(before)) == UMI_STATUS_OK);
    bool success = false, changed = false;
    HANDLE reader = INVALID_HANDLE_VALUE;
    if (strcmp(mode, "sharing-recovered") == 0 || strcmp(mode, "lock-recovered") == 0)
    {
        success = true;
        failures_remaining = 2U;
        injected_error = strcmp(mode, "sharing-recovered") == 0 ? ERROR_SHARING_VIOLATION
                                                               : ERROR_LOCK_VIOLATION;
    }
    else if (strcmp(mode, "sharing-exhausted") == 0 || strcmp(mode, "cleanup-refused") == 0)
    {
        failures_remaining = 32U;
        injected_error = ERROR_SHARING_VIOLATION;
        refuse_cleanup = strcmp(mode, "cleanup-refused") == 0;
    }
    else if (strcmp(mode, "access-denied") == 0 || strcmp(mode, "missing-source") == 0)
    {
        failures_remaining = 32U;
        injected_error = strcmp(mode, "access-denied") == 0 ? ERROR_ACCESS_DENIED : ERROR_FILE_NOT_FOUND;
    }
    else if (strcmp(mode, "destination-changed") == 0)
    {
        changed = true;
        turn_destination_into_directory = true;
        failures_remaining = 32U;
        injected_error = ERROR_SHARING_VIOLATION;
    }
    else if (strcmp(mode, "native-reader") == 0)
    {
        wchar_t wide[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, filename, -1, wide,
                                  (int)UMI_PATH_CAPACITY) != 0);
        /* A real reader retains its handle without FILE_SHARE_DELETE. This
         * exercises the OS boundary in addition to deterministic error cases. */
        reader = CreateFileW(wide, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        CHECK(reader != INVALID_HANDLE_VALUE);
    }
    else
        return 2;
    UmiLocalFileReplaceReport report;
    UmiStatus status = UmiTestLocalFileReplaceDetailed(filename, after, sizeof(after), &report);
    CHECK(report.status == status && report.publish_attempts == move_calls);
    CHECK(report.publish_attempts >= 1U && report.publish_attempts <= 8U);
    if (success)
    {
        CHECK(status == UMI_STATUS_OK && report.stage == UMI_LOCAL_REPLACE_COMPLETE);
        CHECK(move_calls == 3U && wait_calls == 2U && remove_calls == 0U);
        CHECK(!report.cleanup_attempted && !report.cleanup_succeeded);
        CHECK(report.last_publish_error == (uint32_t)injected_error);
    }
    else
    {
        CHECK(status != UMI_STATUS_OK && report.cleanup_attempted);
        CHECK(report.stage == (changed ? UMI_LOCAL_REPLACE_RECHECK : UMI_LOCAL_REPLACE_PUBLISH));
        CHECK(remove_calls == 1U && report.cleanup_succeeded == !refuse_cleanup);
        CHECK(report.last_publish_error != 0U);
        if (reader == INVALID_HANDLE_VALUE)
        {
            CHECK(report.last_publish_error == (uint32_t)injected_error);
            if (changed || injected_error != ERROR_SHARING_VIOLATION)
                CHECK(move_calls == 1U && wait_calls == (changed ? 1U : 0U));
            else
                CHECK(move_calls == 8U && wait_calls == 7U);
        }
    }
    if (reader != INVALID_HANDLE_VALUE)
        CHECK(CloseHandle(reader));
    DWORD attributes = GetFileAttributesW(staging_path);
    CHECK(refuse_cleanup ? attributes != INVALID_FILE_ATTRIBUTES : attributes == INVALID_FILE_ATTRIBUTES);
    if (changed)
    {
        DWORD destination_attributes = GetFileAttributesW(destination_path);
        CHECK(destination_attributes != INVALID_FILE_ATTRIBUTES &&
              (destination_attributes & FILE_ATTRIBUTE_DIRECTORY) != 0U);
    }
    else
    {
        size_t length;
        unsigned char *bytes = FixtureRead(filename, &length);
        CHECK(length == (success ? sizeof(after) : sizeof(before)));
        CHECK(memcmp(bytes, success ? after : before, length) == 0);
        free(bytes);
    }
    return 0;
}
