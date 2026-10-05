/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_rooted_files.c
 * PURPOSE: Check complete rooted file publication, bounded reads and rejected filesystem aliases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/directory.h"
#include <stdint.h>
#include <wchar.h>

static const char unicodeLeaf[] = "caf\xc3\xa9-\xd9\x85\xd9\x84\xd9\x81-\xe6\x96\x87.txt";
static void NativeBytes(const char *root, const char *leaf, const void *expected, size_t size)
{
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, root, leaf);
    size_t actualSize;
    unsigned char *actual = FixtureRead(path, &actualSize);
    CHECK(actualSize == size && memcmp(actual, expected, size) == 0);
    free(actual);
}
static void ReadFailure(const char *root, const char *leaf, size_t maximum, UmiStatus expected)
{
    unsigned char sentinel = 0U, *bytes = &sentinel;
    size_t size = 91U;
    CHECK(UmiRootedFileRead(root, leaf, maximum, &bytes, &size) == expected);
    CHECK(bytes == NULL && size == 0U);
}
static UmiStatus NoStaging(const UmiFileInfo *info, void *data)
{
    (void)data;
    CHECK(strncmp(info->name, ".umicom-write-", 14U) != 0);
    return UMI_STATUS_OK;
}

/* Link fixtures point only at files owned by this test. Unsupported Windows
 * symlink privileges are a reported skip, never an assertion of protection. */
static int Link(const char *linkPath, const char *targetPath, int directory)
{
#ifdef _WIN32
    wchar_t linkName[UMI_PATH_CAPACITY], targetName[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, linkPath, -1, linkName, (int)UMI_PATH_CAPACITY) >
          0);
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, targetPath, -1, targetName,
                              (int)UMI_PATH_CAPACITY) > 0);
    DWORD flags = directory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0U;
    if (CreateSymbolicLinkW(linkName, targetName, flags | 2U))
        return 1;
    DWORD error = GetLastError();
    if (error == ERROR_INVALID_PARAMETER)
    {
        if (CreateSymbolicLinkW(linkName, targetName, flags))
            return 1;
        error = GetLastError();
    }
    if (error == ERROR_PRIVILEGE_NOT_HELD || error == ERROR_NOT_SUPPORTED || error == ERROR_INVALID_FUNCTION)
        return 0;
    CHECK(0);
    return 0;
#else
    (void)directory;
    CHECK(symlink(targetPath, linkPath) == 0);
    return 1;
#endif
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], folder[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(folder, root, "source");
    CHECK(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    CHECK(UmiRootedFileWrite(root, "source/keep.txt", "old", 3U) == UMI_STATUS_OK);
    if (strcmp(mode, "roundtrip") == 0 || strcmp(mode, "replace") == 0 || strcmp(mode, "unicode") == 0)
    {
        const char *leaf = strcmp(mode, "unicode") == 0 ? unicodeLeaf : "source/keep.txt";
        const unsigned char value[] = {0U, 10U, 13U, 255U, 27U, 128U};
        CHECK(UmiRootedFileWrite(root, leaf, value, sizeof(value)) == UMI_STATUS_OK);
        NativeBytes(root, leaf, value, sizeof(value));
        unsigned char *bytes = NULL;
        size_t size = 0U;
        CHECK(UmiRootedFileRead(root, leaf, sizeof(value), &bytes, &size) == UMI_STATUS_OK);
        CHECK(size == sizeof(value) && memcmp(bytes, value, size) == 0 && bytes[size] == 0U);
        UmiRootedFileFree(bytes);
        if (strcmp(mode, "replace") == 0)
        {
            CHECK(UmiRootedFileWrite(root, leaf, "x", 1U) == UMI_STATUS_OK);
            NativeBytes(root, leaf, "x", 1U);
        }
    }
    else if (strcmp(mode, "empty") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "empty.txt", NULL, 0U) == UMI_STATUS_OK);
        unsigned char *bytes = NULL;
        size_t size = 1U;
        CHECK(UmiRootedFileRead(root, "empty.txt", 0U, &bytes, &size) == UMI_STATUS_OK);
        CHECK(bytes != NULL && size == 0U && bytes[0] == 0U);
        UmiRootedFileFree(bytes);
    }
    else if (strcmp(mode, "limit") == 0)
    {
        ReadFailure(root, "source/keep.txt", 2U, UMI_STATUS_CAPACITY_EXCEEDED);
        ReadFailure(root, "source/keep.txt", SIZE_MAX, UMI_STATUS_INVALID_ARGUMENT);
        NativeBytes(root, "source/keep.txt", "old", 3U);
    }
    else if (strcmp(mode, "missing") == 0)
    {
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "absent", &exists) == UMI_STATUS_OK && !exists);
        ReadFailure(root, "absent", 30U, UMI_STATUS_NOT_FOUND);
        CHECK(UmiRootedFileRemove(root, "absent") == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(mode, "missing-parent") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "absent/child", "x", 1U) == UMI_STATUS_NOT_FOUND);
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "absent/child", &exists) == UMI_STATUS_OK && !exists);
        FixturePath(path, root, "absent");
        CHECK(!umi_fs_exists(path));
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        const char *invalid[] = {"../escape",    "source/../keep", "/absolute",     "C:relative",
                                 ".git/config",  "source//keep",   "source/./keep", "source/keep.",
                                 "source/keep ", "NUL.txt",        "COM1",          "LPT0.txt",
                                 "CONIN$",       "CONOUT$.txt",    "COM\xc2\xb9",   "LPT\xc2\xb2.txt",
                                 "COM\xc2\xb3",  "CON .txt",       "a:stream"};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
        {
            CHECK(UmiRootedFileWrite(root, invalid[i], "x", 1U) != UMI_STATUS_OK);
            int exists = 1;
            CHECK(UmiRootedFileExists(root, invalid[i], &exists) != UMI_STATUS_OK && !exists);
        }
        CHECK(UmiRootedFileWrite(".", "keep", "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiRootedFileWrite(NULL, "keep", "x", 1U) == UMI_STATUS_INVALID_ARGUMENT);
        NativeBytes(root, "source/keep.txt", "old", 3U);
    }
    else if (strcmp(mode, "invalid-write") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "source/keep.txt", NULL, 3U) == UMI_STATUS_INVALID_ARGUMENT);
        NativeBytes(root, "source/keep.txt", "old", 3U);
    }
    else if (strcmp(mode, "directory") == 0)
    {
        CHECK(UmiRootedFileWrite(root, "source", "x", 1U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiRootedFileRemove(root, "source") == UMI_STATUS_PERMISSION_DENIED);
        ReadFailure(root, "source", 30U, UMI_STATUS_PERMISSION_DENIED);
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "source", &exists) == UMI_STATUS_PERMISSION_DENIED && !exists);
        NativeBytes(root, "source/keep.txt", "old", 3U);
    }
    else if (strcmp(mode, "remove") == 0)
    {
        CHECK(UmiRootedFileRemove(root, "source/keep.txt") == UMI_STATUS_OK);
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "source/keep.txt", &exists) == UMI_STATUS_OK && !exists);
        CHECK(umi_fs_is_directory(folder));
    }
    else if (strcmp(mode, "staging") == 0)
    {
        for (unsigned i = 0U; i < 5U; ++i)
            CHECK(UmiRootedFileWrite(root, "source/keep.txt", &i, sizeof(i)) == UMI_STATUS_OK);
        UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
        options.include_hidden = 1;
        CHECK(umi_directory_walk(folder, &options, NoStaging, NULL) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "staging-collision") == 0 || strcmp(mode, "staging-name") == 0)
    {
#ifdef _WIN32
        unsigned long process = (unsigned long)GetCurrentProcessId();
#else
        unsigned long process = (unsigned long)getpid();
#endif
        /* The fixture's first completed write consumed sequence zero. Reserve
         * the next name independently to exercise exclusive-create retry. */
        char leaf[96];
        int length = snprintf(leaf, sizeof(leaf), ".umicom-write-%lu-1", process);
        CHECK(length > 0 && (size_t)length < sizeof(leaf));
        FixturePath(path, root, leaf);
        if (strcmp(mode, "staging-collision") == 0)
        {
            CHECK(umi_fs_write_text(path, "occupied") == UMI_STATUS_OK);
            CHECK(UmiRootedFileWrite(root, "published.txt", "complete", 8U) == UMI_STATUS_OK);
            NativeBytes(root, leaf, "occupied", 8U);
            NativeBytes(root, "published.txt", "complete", 8U);
        }
        else
        {
            CHECK(UmiRootedFileWrite(root, leaf, "complete", 8U) == UMI_STATUS_OK);
            NativeBytes(root, leaf, "complete", 8U);
        }
    }
    else if (strcmp(mode, "locked") == 0)
    {
#ifdef _WIN32
        FixturePath(path, folder, "keep.txt");
        wchar_t native[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, native, (int)UMI_PATH_CAPACITY) >
              0);
        HANDLE file = CreateFileW(native, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, NULL);
        CHECK(file != INVALID_HANDLE_VALUE);
        CHECK(UmiRootedFileWrite(root, "source/keep.txt", "new", 3U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(CloseHandle(file));
        NativeBytes(root, "source/keep.txt", "old", 3U);
        UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
        options.include_hidden = 1;
        CHECK(umi_directory_walk(folder, &options, NoStaging, NULL) == UMI_STATUS_OK);
#else
        return 77;
#endif
    }
    else if (strcmp(mode, "large") == 0)
    {
        size_t length = 1048576U + 37U;
        unsigned char *input = malloc(length);
        CHECK(input != NULL);
        for (size_t i = 0U; i < length; ++i)
            input[i] = (unsigned char)(i % 251U);
        CHECK(UmiRootedFileWrite(root, "large.bin", input, length) == UMI_STATUS_OK);
        unsigned char *output = NULL;
        size_t size = 0U;
        CHECK(UmiRootedFileRead(root, "large.bin", length, &output, &size) == UMI_STATUS_OK);
        CHECK(size == length && memcmp(input, output, length) == 0);
        free(input);
        UmiRootedFileFree(output);
    }
    else if (strcmp(mode, "leaf-link") == 0 || strcmp(mode, "parent-link") == 0 ||
             strcmp(mode, "root-link") == 0)
    {
        FixturePath(path, root, "alias");
        char target[UMI_PATH_CAPACITY];
        FixturePath(target, folder, "keep.txt");
        int directory = strcmp(mode, "leaf-link") != 0;
        if (!Link(path, directory ? folder : target, directory))
            return 77;
        const char *selectedRoot = strcmp(mode, "root-link") == 0 ? path : root;
        const char *leaf = strcmp(mode, "root-link") == 0 ? "keep.txt"
                           : directory                    ? "alias/keep.txt"
                                                          : "alias";
        char trailing[UMI_PATH_CAPACITY];
        if (strcmp(mode, "root-link") == 0)
        {
            int length = snprintf(trailing, sizeof(trailing), "%s/", selectedRoot);
            CHECK(length > 0 && (size_t)length < sizeof(trailing));
            selectedRoot = trailing;
        }
        ReadFailure(selectedRoot, leaf, 30U, UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiRootedFileWrite(selectedRoot, leaf, "new", 3U) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiRootedFileRemove(selectedRoot, leaf) == UMI_STATUS_PERMISSION_DENIED);
        NativeBytes(root, "source/keep.txt", "old", 3U);
    }
    else if (strcmp(mode, "readonly") == 0)
    {
#ifdef _WIN32
        FixturePath(path, folder, "keep.txt");
        wchar_t native[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, native, (int)UMI_PATH_CAPACITY) >
              0);
        CHECK(SetFileAttributesW(native, FILE_ATTRIBUTE_READONLY));
        CHECK(UmiRootedFileWrite(root, "source/keep.txt", "new", 3U) == UMI_STATUS_PERMISSION_DENIED);
        NativeBytes(root, "source/keep.txt", "old", 3U);
        CHECK(SetFileAttributesW(native, FILE_ATTRIBUTE_NORMAL));
#else
        return 77;
#endif
    }
    else if (strcmp(mode, "permissions") == 0)
    {
#ifndef _WIN32
        FixturePath(path, folder, "keep.txt");
        CHECK(chmod(path, 0750) == 0);
        CHECK(UmiRootedFileWrite(root, "source/keep.txt", "new", 3U) == UMI_STATUS_OK);
        struct stat info;
        CHECK(stat(path, &info) == 0 && (info.st_mode & 0777) == 0750);
#else
        return 77;
#endif
    }
    else if (strcmp(mode, "hardlink") == 0)
    {
        FixturePath(path, root, "alias.txt");
        char target[UMI_PATH_CAPACITY];
        FixturePath(target, folder, "keep.txt");
#ifdef _WIN32
        wchar_t native[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY];
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, native, (int)UMI_PATH_CAPACITY) >
              0);
        CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, target, -1, source, (int)UMI_PATH_CAPACITY) >
              0);
        CHECK(CreateHardLinkW(native, source, NULL));
#else
        CHECK(link(target, path) == 0);
#endif
        CHECK(UmiRootedFileWrite(root, "source/keep.txt", "new", 3U) == UMI_STATUS_OK);
        NativeBytes(root, "source/keep.txt", "new", 3U);
        NativeBytes(root, "alias.txt", "old", 3U);
    }
    else
        return 2;
    return 0;
}
