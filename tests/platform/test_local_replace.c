/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_local_replace.c
 * PURPOSE: Verify replacement, retained data on refusal and Unicode paths in isolated directories.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/platform/local_replace.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, strcmp(mode, "unicode") == 0 ? "caf\xc3\xa9.settings" : "settings.json");
    const unsigned char before[] = {1U, 0U, 2U, 3U}, after[] = {7U, 0U, 9U};
    CHECK(UmiLocalFileReplace(path, before, sizeof(before)) == UMI_STATUS_OK);
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
        return 77;
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
        for (size_t i = 0U; i < count; ++i)
            CHECK(UmiLocalFileReplace(path, after, sizeof(after)) == UMI_STATUS_OK);
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
