/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/platform/test_rooted_create.c
 *
 * PURPOSE:
 *   Check exclusive new-file writes, retained content and rooted path refusal.
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
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"

static void CheckBytes(const char *root, const char *leaf, const void *expected, size_t count)
{
    char path[UMI_PATH_CAPACITY];
    FixturePath(path, root, leaf);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(path, &size);
    CHECK(size == count && memcmp(bytes, expected, count) == 0);
    free(bytes);
}
static void Refused(const char *root, const char *leaf, UmiStatus expected)
{
    UmiRootedFileCreation report = {99U, 1, 1, UMI_STATUS_OK};
    CHECK(UmiRootedFileCreate(root, leaf, "new", 3U, &report) == expected);
    CHECK(!report.created && !report.complete && report.bytes_written == 0U);
    CHECK(report.status == expected);
}
static int MakeLink(const char *alias, const char *target, int directory)
{
#ifdef _WIN32
    return FixtureSymbolicLink(alias, target, directory);
#else
    (void)directory;
    CHECK(symlink(target, alias) == 0);
    return 1;
#endif
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], folder[UMI_PATH_CAPACITY], target[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(folder, root, "practice");
    CHECK(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    const unsigned char value[] = {0U, 'C', '\n', 0xffU, 0U};
    UmiRootedFileCreation report = {0};
    if (strcmp(mode, "roundtrip") == 0 || strcmp(mode, "empty") == 0 ||
        strcmp(mode, "unicode") == 0 || strcmp(mode, "large") == 0) {
        const char *leaf = strcmp(mode, "unicode") == 0 ?
            "practice/caf\xc3\xa9-\xe6\x96\x87.c" : "practice/new.c";
        size_t size = strcmp(mode, "empty") == 0 ? 0U : sizeof(value);
        unsigned char *large = NULL;
        const void *bytes = size == 0U ? NULL : value;
        if (strcmp(mode, "large") == 0) {
            size = 1048576U + 17U;
            large = malloc(size);
            CHECK(large != NULL);
            for (size_t i = 0U; i < size; ++i) large[i] = (unsigned char)(i % 251U);
            bytes = large;
        }
        CHECK(UmiRootedFileCreate(root, leaf, bytes, size, &report) == UMI_STATUS_OK);
        CHECK(report.created && report.complete && report.bytes_written == size && report.status == UMI_STATUS_OK);
        unsigned char *received = NULL;
        size_t received_size = 0U;
        CHECK(UmiRootedFileRead(root, leaf, size, &received, &received_size) == UMI_STATUS_OK);
        CHECK(received_size == size && (size == 0U || memcmp(received, bytes, size) == 0));
        UmiRootedFileFree(received);
        /* A second call refuses even exactly identical bytes. */
        Refused(root, leaf, UMI_STATUS_ALREADY_EXISTS);
        free(large);
    } else if (strcmp(mode, "existing") == 0) {
        CHECK(UmiRootedFileWrite(root, "practice/old.c", value, sizeof(value)) == UMI_STATUS_OK);
        Refused(root, "practice/old.c", UMI_STATUS_ALREADY_EXISTS);
        CheckBytes(root, "practice/old.c", value, sizeof(value));
    } else if (strcmp(mode, "missing-parent") == 0) {
        Refused(root, "absent/new.c", UMI_STATUS_NOT_FOUND);
    } else if (strcmp(mode, "directory") == 0) {
        UmiStatus status = UmiRootedFileCreate(root, "practice", value, sizeof(value), &report);
        CHECK(status != UMI_STATUS_OK && !report.created && !report.complete && report.bytes_written == 0U);
    } else if (strcmp(mode, "arguments") == 0) {
        Refused(NULL, "new.c", UMI_STATUS_INVALID_ARGUMENT);
        Refused(root, NULL, UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiRootedFileCreate(root, "new.c", NULL, 3U, &report) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!report.created && !report.complete && report.bytes_written == 0U);
        CHECK(UmiRootedFileCreate(root, "new.c", value, sizeof(value), NULL) == UMI_STATUS_INVALID_ARGUMENT);
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "new.c", &exists) == UMI_STATUS_OK && !exists);
    } else if (strcmp(mode, "traversal") == 0) {
        Refused(root, "../escape.c", UMI_STATUS_INVALID_ARGUMENT);
        Refused(root, "practice/../../escape.c", UMI_STATUS_INVALID_ARGUMENT);
        Refused(root, "practice\\..\\escape.c", UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "absolute") == 0) {
        Refused(root, "/escape.c", UMI_STATUS_INVALID_ARGUMENT);
        Refused(root, "C:/escape.c", UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "git") == 0) {
        Refused(root, ".git/config", UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(mode, "device") == 0) {
        Refused(root, "NUL.c", UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(mode, "leaf-link") == 0 || strcmp(mode, "parent-link") == 0 ||
               strcmp(mode, "root-link") == 0) {
        CHECK(UmiRootedFileWrite(root, "practice/keep.c", value, sizeof(value)) == UMI_STATUS_OK);
        char alias[UMI_PATH_CAPACITY];
        FixturePath(alias, root, "alias");
        FixturePath(target, root, strcmp(mode, "leaf-link") == 0 ? "practice/keep.c" : "practice");
        if (!MakeLink(alias, target, strcmp(mode, "leaf-link") != 0)) {
            fprintf(stderr, "SKIP: symbolic-link creation is unavailable for this fixture.\n");
            return 77;
        }
        UmiStatus status = strcmp(mode, "root-link") == 0 ?
            UmiRootedFileCreate(alias, "new.c", value, sizeof(value), &report) :
            UmiRootedFileCreate(root, strcmp(mode, "leaf-link") == 0 ? "alias" : "alias/new.c",
                value, sizeof(value), &report);
        CHECK(status != UMI_STATUS_OK && !report.created && !report.complete && report.bytes_written == 0U);
        CheckBytes(root, "practice/keep.c", value, sizeof(value));
        int exists = 1;
        CHECK(UmiRootedFileExists(root, "practice/new.c", &exists) == UMI_STATUS_OK && !exists);
    } else return 2;
    return 0;
}
