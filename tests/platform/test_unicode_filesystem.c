/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_unicode_filesystem.c
 * PURPOSE: Exercise public filesystem workflows using native Unicode names and independent byte checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/resource_location.h"
#include <wchar.h>

/* Escape UTF-8 bytes explicitly so the source compiler's execution character
 * set cannot change what the public API receives. Include a supplementary
 * character, Arabic and CJK text, not only names in a Western code page. */
static const char unicodeName[] = "caf\xc3\xa9-\xd9\x85\xd9\x84\xd9\x81-\xe6\x96\x87-\xf0\x9f\x93\x81";

static void CheckBytes(const char *path, const void *expected, size_t size)
{
    size_t actualSize;
    unsigned char *native = FixtureRead(path, &actualSize);
    CHECK(actualSize == size && memcmp(native, expected, size) == 0);
    free(native);
    unsigned char *portable = NULL;
    CHECK(umi_fs_read_bytes(path, &portable, &actualSize) == UMI_STATUS_OK);
    CHECK(actualSize == size && memcmp(portable, expected, size) == 0);
    CHECK(portable[size] == 0U);
    umi_fs_free_bytes(portable);
}

typedef struct Listing
{
    size_t files, directories, names;
    int foundUnicode;
    UmiCancellationToken *cancel;
} Listing;

static UmiStatus Visit(const UmiFileInfo *info, void *user)
{
    Listing *listing = user;
    CHECK(umi_fs_exists(info->path));
    if (info->kind == UMI_FILE_KIND_DIRECTORY)
        ++listing->directories;
    if (info->kind == UMI_FILE_KIND_REGULAR)
    {
        ++listing->files;
        CHECK(info->size == 3U);
        CheckBytes(info->path, "abc", 3U);
    }
    if (strcmp(info->name, unicodeName) == 0)
        listing->foundUnicode = 1;
    ++listing->names;
    if (listing->cancel != NULL)
        umi_cancellation_token_request(listing->cancel);
    return UMI_STATUS_OK;
}

#ifdef _WIN32
static void Wide(const char *path, wchar_t *out)
{
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, out, (int)UMI_PATH_CAPACITY) > 0);
}

static wchar_t *Environment(const wchar_t *name)
{
    DWORD count = GetEnvironmentVariableW(name, NULL, 0U);
    if (count == 0U)
        return NULL;
    wchar_t *value = calloc((size_t)count, sizeof(*value));
    CHECK(value != NULL && GetEnvironmentVariableW(name, value, count) < count);
    return value;
}
#endif

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(directory, root, unicodeName);
    CHECK(umi_fs_make_directories(directory) == UMI_STATUS_OK);
    CHECK(umi_fs_exists(directory) && umi_fs_is_directory(directory) && !umi_fs_is_file(directory));
    FixturePath(path, directory, unicodeName);
    if (strcmp(mode, "application-paths") == 0)
    {
        /* Exercise the actual per-user directory preparation service with an
         * isolated override; this never touches the user's application data. */
        UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault("Studio");
        config.baseOverride = directory;
        UmiApplicationPaths *paths = calloc(1U, sizeof(*paths));
        CHECK(paths != NULL && UmiApplicationPathsResolve(&config, paths) == UMI_STATUS_OK);
        CHECK(UmiApplicationPathsPrepare(paths) == UMI_STATUS_OK);
        CHECK(umi_fs_is_directory(paths->config) && umi_fs_is_directory(paths->recovery));
        FixturePath(path, paths->config, unicodeName);
        CHECK(umi_fs_write_text(path, "settings") == UMI_STATUS_OK);
        CheckBytes(path, "settings", 8U);
        free(paths);
    }
    else if (strcmp(mode, "roundtrip") == 0)
    {
        unsigned char data[16431];
        for (size_t i = 0U; i < sizeof(data); ++i)
            data[i] = (unsigned char)(i % 251U);
        CHECK(umi_fs_write_bytes(path, data, sizeof(data)) == UMI_STATUS_OK);
        CheckBytes(path, data, sizeof(data));
        CHECK(umi_fs_is_file(path) && !umi_fs_is_directory(path));
    }
    else if (strcmp(mode, "append") == 0)
    {
        CHECK(umi_fs_write_text(path, "one\r\n") == UMI_STATUS_OK);
        CHECK(umi_fs_append_text(path, "two\n") == UMI_STATUS_OK);
        CheckBytes(path, "one\r\ntwo\n", 9U);
        char *text = NULL;
        size_t count;
        CHECK(umi_fs_read_text(path, &text, &count) == UMI_STATUS_OK);
        CHECK(count == 9U && strcmp(text, "one\r\ntwo\n") == 0);
        umi_fs_free_text(text);
    }
    else if (strcmp(mode, "copy") == 0)
    {
        char nested[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
        FixturePath(nested, directory, "new-parent");
        FixturePath(destination, nested, unicodeName);
        CHECK(umi_fs_write_bytes(path, "a\0b", 3U) == UMI_STATUS_OK);
        CHECK(umi_fs_copy_file(path, destination) == UMI_STATUS_OK);
        CheckBytes(path, "a\0b", 3U);
        CheckBytes(destination, "a\0b", 3U);
    }
    else if (strcmp(mode, "rename") == 0)
    {
        char destination[UMI_PATH_CAPACITY];
        FixturePath(destination, root, "renamed.txt");
        CHECK(umi_fs_write_text(path, "unchanged") == UMI_STATUS_OK);
        CHECK(umi_fs_rename(path, destination) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(path));
        CheckBytes(destination, "unchanged", 9U);
        CHECK(umi_fs_rename(destination, path) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(destination));
        CheckBytes(path, "unchanged", 9U);
    }
    else if (strcmp(mode, "directories") == 0)
    {
        char nested[UMI_PATH_CAPACITY];
        FixturePath(nested, path, unicodeName);
        CHECK(umi_fs_make_directories(nested) == UMI_STATUS_OK);
        CHECK(umi_fs_make_directories(nested) == UMI_STATUS_OK && umi_fs_is_directory(nested));
#ifdef _WIN32
        char collision[UMI_PATH_CAPACITY];
        FixturePath(collision, directory, "existing-file");
        CHECK(umi_fs_write_text(collision, "retain") == UMI_STATUS_OK);
        CHECK(umi_fs_make_directories(collision) != UMI_STATUS_OK);
        CheckBytes(collision, "retain", 6U);
#endif
    }
    else if (strcmp(mode, "metadata") == 0)
    {
        UmiFileInfo info;
        CHECK(umi_fs_write_bytes(path, "a\0b", 3U) == UMI_STATUS_OK);
        CHECK(umi_directory_stat(path, &info) == UMI_STATUS_OK);
        CHECK(strcmp(info.name, unicodeName) == 0 && info.kind == UMI_FILE_KIND_REGULAR && info.size == 3U);
        CHECK(umi_directory_stat(directory, &info) == UMI_STATUS_OK && info.kind == UMI_FILE_KIND_DIRECTORY);
    }
    else if (strcmp(mode, "walk") == 0 || strcmp(mode, "cancel") == 0)
    {
        char child[UMI_PATH_CAPACITY];
        FixturePath(child, directory, "second.txt");
        CHECK(umi_fs_write_text(path, "abc") == UMI_STATUS_OK &&
              umi_fs_write_text(child, "abc") == UMI_STATUS_OK);
        UmiDirectoryWalkOptions options = umi_directory_walk_options_default();
        options.include_directories = 1;
        Listing listing = {0};
        if (strcmp(mode, "cancel") == 0)
        {
            CHECK(umi_cancellation_token_create(&listing.cancel) == UMI_STATUS_OK);
            CHECK(UmiDirectoryWalkCancellable(root, &options, Visit, &listing, listing.cancel) ==
                  UMI_STATUS_CANCELLED);
            CHECK(listing.names == 1U);
            umi_cancellation_token_destroy(listing.cancel);
        }
        else
        {
            CHECK(umi_directory_walk(root, &options, Visit, &listing) == UMI_STATUS_OK);
            CHECK(listing.files == 2U && listing.directories == 1U && listing.foundUnicode);
        }
    }
    else if (strcmp(mode, "remove") == 0)
    {
        CHECK(umi_fs_write_text(path, "abc") == UMI_STATUS_OK);
        CHECK(umi_fs_remove_tree(directory) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(directory) && umi_fs_exists(root));
        CHECK(umi_fs_remove_tree(directory) == UMI_STATUS_OK);
    }
    else if (strcmp(mode, "current") == 0)
    {
#ifdef _WIN32
        wchar_t before[UMI_PATH_CAPACITY], target[UMI_PATH_CAPACITY];
        DWORD count = GetCurrentDirectoryW(UMI_PATH_CAPACITY, before);
        CHECK(count != 0U && count < UMI_PATH_CAPACITY);
        Wide(directory, target);
        CHECK(SetCurrentDirectoryW(target));
        char actual[UMI_PATH_CAPACITY];
        CHECK(umi_fs_current_directory(actual, sizeof(actual)) == UMI_STATUS_OK);
        CHECK(umi_path_equal(actual, directory));
        char small[2] = "!";
        CHECK(umi_fs_current_directory(small, sizeof(small)) == UMI_STATUS_CAPACITY_EXCEEDED &&
              strcmp(small, "!") == 0);
        CHECK(umi_fs_write_text(unicodeName, "relative") == UMI_STATUS_OK);
        CHECK(SetCurrentDirectoryW(before));
        CheckBytes(path, "relative", 8U);
#else
        return 77;
#endif
    }
    else if (strcmp(mode, "temporary") == 0)
    {
#ifdef _WIN32
        wchar_t *beforeTemp = Environment(L"TEMP"), *beforeTmp = Environment(L"TMP"),
                target[UMI_PATH_CAPACITY];
        Wide(directory, target);
        CHECK(SetEnvironmentVariableW(L"TEMP", target));
        char actual[UMI_PATH_CAPACITY];
        CHECK(umi_fs_temp_directory(actual, sizeof(actual)) == UMI_STATUS_OK &&
              strcmp(actual, directory) == 0);
        char small[2] = "!";
        CHECK(umi_fs_temp_directory(small, sizeof(small)) == UMI_STATUS_CAPACITY_EXCEEDED &&
              strcmp(small, "!") == 0);
        CHECK(SetEnvironmentVariableW(L"TEMP", NULL) && SetEnvironmentVariableW(L"TMP", target));
        CHECK(umi_fs_temp_directory(actual, sizeof(actual)) == UMI_STATUS_OK &&
              strcmp(actual, directory) == 0);
        CHECK(SetEnvironmentVariableW(L"TMP", NULL));
        CHECK(umi_fs_temp_directory(actual, sizeof(actual)) == UMI_STATUS_OK && strcmp(actual, ".") == 0);
        CHECK(SetEnvironmentVariableW(L"TEMP", beforeTemp) && SetEnvironmentVariableW(L"TMP", beforeTmp));
        free(beforeTemp);
        free(beforeTmp);
#else
        return 77;
#endif
    }
    else if (strcmp(mode, "invalid") == 0)
    {
#ifdef _WIN32
        char invalid[UMI_PATH_CAPACITY];
        FixturePath(invalid, directory, "invalid-\xc0\xaf");
        CHECK(umi_fs_write_text(path, "retain") == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(invalid, "deny") != UMI_STATUS_OK);
        CHECK(!umi_fs_exists(invalid));
        CHECK(umi_fs_make_directories(invalid) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_fs_remove_tree(invalid) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_fs_rename(path, invalid) == UMI_STATUS_INVALID_ARGUMENT);
        UmiFileInfo info;
        CHECK(umi_directory_stat(invalid, &info) == UMI_STATUS_INVALID_ARGUMENT);
        CheckBytes(path, "retain", 6U);
#else
        return 77;
#endif
    }
    else if (strcmp(mode, "reparse") == 0)
    {
#ifdef _WIN32
        char outside[UMI_PATH_CAPACITY], sentinel[UMI_PATH_CAPACITY];
        FixtureDirectory(outside);
        FixturePath(sentinel, outside, unicodeName);
        CHECK(umi_fs_write_text(sentinel, "retain") == UMI_STATUS_OK);
        wchar_t nativeLink[UMI_PATH_CAPACITY], nativeTarget[UMI_PATH_CAPACITY];
        Wide(path, nativeLink);
        Wide(outside, nativeTarget);
        if (!CreateSymbolicLinkW(nativeLink, nativeTarget, SYMBOLIC_LINK_FLAG_DIRECTORY | 2U))
        {
            DWORD error = GetLastError();
            if (error == ERROR_PRIVILEGE_NOT_HELD || error == ERROR_INVALID_PARAMETER ||
                error == ERROR_NOT_SUPPORTED)
                return 77;
            CHECK(0);
        }
        UmiFileInfo info;
        CHECK(umi_directory_stat(path, &info) == UMI_STATUS_OK && info.kind == UMI_FILE_KIND_SYMBOLIC_LINK);
        CHECK(umi_fs_remove_tree(directory) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(directory));
        CheckBytes(sentinel, "retain", 6U);
#else
        return 77;
#endif
    }
    else
        return 2;
    return 0;
}
