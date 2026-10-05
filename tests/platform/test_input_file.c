/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_input_file.c
 * PURPOSE: Read isolated native files under strict limits, including binary bytes, Unicode names and non-regular leaves.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* Reuse the existing exclusive fixture-directory owner. It leaves only files
 * it created under the test working directory and never reuses an old leaf. */
#include "../build_log/fixture.h"
#include "umicom/platform/input_file.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"bytes",   "empty",     "unicode",   "limit", "large",
                           "missing", "directory", "arguments", "fifo",  "symlink"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(path, root, strcmp(name, "unicode") == 0 ? "caf\xc3\xa9.txt" : "input.txt");
    unsigned char *out = NULL;
    size_t size = 99U;
    if (strcmp(name, "arguments") == 0)
    {
        CHECK(UmiInputFileRead(NULL, 10U, &out, &size) == UMI_STATUS_INVALID_ARGUMENT && out == NULL &&
              size == 0U);
        CHECK(UmiInputFileRead("relative", 10U, &out, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiInputFileRead(path, SIZE_MAX, &out, &size) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiInputFileRead(path, 10U, NULL, &size) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(name, "missing") == 0)
    {
        CHECK(UmiInputFileRead(path, 10U, &out, &size) == UMI_STATUS_NOT_FOUND && out == NULL && size == 0U);
        return 0;
    }
    if (strcmp(name, "directory") == 0)
    {
        CHECK(UmiInputFileRead(root, 10U, &out, &size) != UMI_STATUS_OK && out == NULL && size == 0U);
        return 0;
    }
    if (strcmp(name, "fifo") == 0)
    {
#ifdef _WIN32
        return 77;
#else
        CHECK(mkfifo(path, 0600) == 0);
        CHECK(UmiInputFileRead(path, 10U, &out, &size) != UMI_STATUS_OK && out == NULL);
        return 0;
#endif
    }
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
    size_t expected = strcmp(name, "empty") == 0 ? 0U : strcmp(name, "large") == 0 ? 131077U : 5U;
    unsigned char *bytes = calloc(expected + 1U, 1U);
    CHECK(bytes != NULL);
    for (size_t i = 0U; i < expected; ++i)
        bytes[i] = (unsigned char)(i % 256U);
    CHECK(UmiOutputFileWrite(file, bytes, expected) == UMI_STATUS_OK);
    CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
    if (strcmp(name, "symlink") == 0)
    {
        char link[UMI_PATH_CAPACITY];
        FixturePath(link, root, "linked.txt");
#ifdef _WIN32
        wchar_t widePath[UMI_PATH_CAPACITY], wideLink[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, widePath,
                                  (int)UMI_PATH_CAPACITY) != 0);
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, link, -1, wideLink,
                                  (int)UMI_PATH_CAPACITY) != 0);
        if (!CreateSymbolicLinkW(wideLink, widePath, 2U))
        {
            free(bytes);
            return 77;
        }
#else
        CHECK(symlink(path, link) == 0);
#endif
        CHECK(UmiInputFileRead(link, expected, &out, &size) != UMI_STATUS_OK && out == NULL && size == 0U);
        free(bytes);
        return 0;
    }
    if (strcmp(name, "limit") == 0)
    {
        CHECK(UmiInputFileRead(path, expected - 1U, &out, &size) == UMI_STATUS_CAPACITY_EXCEEDED &&
              out == NULL && size == 0U);
    }
    else
    {
        CHECK(UmiInputFileRead(path, expected, &out, &size) == UMI_STATUS_OK);
        CHECK(size == expected && memcmp(out, bytes, size) == 0 && out[size] == 0U);
    }
    UmiInputFileFree(out);
    free(bytes);
    return 0;
}
