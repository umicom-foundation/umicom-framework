/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_local_replace.c
 *
 * PURPOSE:
 *   Verify replacement, retained data on refusal and Unicode paths in isolated directories.
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
#include "umicom/platform/local_replace.h"
/* Report the operation and iteration before the assertion. A native error is
 * meaningful only when captured by the replacement service itself; reading the
 * thread error after cleanup could identify an unrelated operation instead. */
static UmiStatus ReplaceWithEvidence(const char *path, const void *bytes, size_t length, size_t iteration)
{
    UmiLocalFileReplaceReport report;
    UmiStatus status = UmiLocalFileReplaceDetailed(path, bytes, length, &report);
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Replacement iteration %zu: status=%d stage=%d publish-error=%lu "
                        "attempts=%u cleanup=%d/%d\n",
                iteration, (int)status, (int)report.stage, (unsigned long)report.last_publish_error,
                report.publish_attempts, (int)report.cleanup_attempted, (int)report.cleanup_succeeded);
    return status;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9.settings" : "settings.json");
    const unsigned char before[] = {1U, 0U, 2U, 3U}, after[] = {7U, 0U, 9U};
    /* Retain the former assertion for review; evidence now precedes failure. */
#if 0
    CHECK(UmiLocalFileReplace(path, before, sizeof(before)) == UMI_STATUS_OK);
#endif
    CHECK(ReplaceWithEvidence(path, before, sizeof(before), 0U) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid") == 0)
    {
        CHECK(UmiLocalFileReplace(path, NULL, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLocalFileReplace("relative", after, sizeof(after)) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "directory") == 0)
    {
        CHECK(UmiLocalFileReplace(root, after, sizeof(after)) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "symlink") == 0)
    {
#ifdef _WIN32
        /* Exercise the same refusal contract on Windows when link creation is
         * available; the previous platform-only skip is retained for review. */
#if 0
        return 77;
#endif
        char link[UMI_PATH_CAPACITY];
        FixturePath(link, root, "linked");
        if (!FixtureSymbolicLink(link, path, 0)) return 77;
        CHECK(UmiLocalFileReplace(link, after, sizeof(after)) == UMI_STATUS_INVALID_ARGUMENT);
#else
        char link[UMI_PATH_CAPACITY];
        FixturePath(link, root, "linked");
        CHECK(symlink(path, link) == 0);
        CHECK(UmiLocalFileReplace(link, after, sizeof(after)) == UMI_STATUS_INVALID_ARGUMENT);
#endif
    }
    else if (strcmp(mode, "missing-parent") == 0)
    {
        char missing[UMI_PATH_CAPACITY];
        FixturePath(missing, root, "absent/settings");
        CHECK(UmiLocalFileReplace(missing, after, sizeof(after)) != UMI_STATUS_OK);
    }
    else if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiLocalFileReplace(path, NULL, 0U) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "replace") == 0 || strcmp(mode, "unicode") == 0 || strcmp(mode, "repeat") == 0)
    {
        size_t count = strcmp(mode, "repeat") == 0 ? 100U : 1U;
        /* Retain the original loop while identifying the failing iteration in
         * the active replacement path. The same byte assertions still apply. */
#if 0
        for (size_t i = 0U; i < count; ++i)
            CHECK(UmiLocalFileReplace(path, after, sizeof(after)) == UMI_STATUS_OK);
#endif
        for (size_t i = 0U; i < count; ++i)
            CHECK(ReplaceWithEvidence(path, after, sizeof(after), i + 1U) == UMI_STATUS_OK);
    }
    else
        return 2;
    size_t size;
    unsigned char *read = FixtureRead(path, &size);
    int replaced =
        strcmp(mode, "replace") == 0 || strcmp(mode, "unicode") == 0 || strcmp(mode, "repeat") == 0;
    size_t expected = strcmp(mode, "empty") == 0 ? 0U : replaced ? sizeof(after) : sizeof(before);
    CHECK(size == expected && memcmp(read, replaced ? after : before, expected) == 0);
    free(read);
    return 0;
}
