/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_library/test_file.c
 * PURPOSE: Exercise native collection persistence and leave prior files and captures intact on refused operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/creative_workspace/asset_library_archive.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    const char *cases[] = {"roundtrip",     "existing",  "cancelled-save", "cancelled-load", "relative-save",
                           "relative-load", "missing",   "directory",      "payload-limit",  "source-privacy",
                           "unicode-path",  "truncated", "corrupt",        "byte-equality",  "live-output"};
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY],
        missing[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    const char *leaf = strcmp(mode, "unicode-path") == 0 ? "caf\xc3\xa9.umilibrary" : "project.umilibrary";
    FixturePath(path, root, leaf);
    FixturePath(source, root, "private-source.bin");
    FixturePath(missing, root, "missing.umilibrary");
    CHECK(UmiRootedFileWrite(root, "private-source.bin", "one\0two", 7U) == UMI_STATUS_OK);
    UmiCreativeAsset *asset = NULL;
    UmiCreativeAssetLibrary *library = NULL, *loaded = NULL;
    CHECK(UmiCreativeAssetLoadFile(source, "Image", UMI_CREATIVE_ASSET_IMAGE, 8U, NULL, &asset) ==
          UMI_STATUS_OK);
    CHECK(UmiCreativeAssetLibraryCreate("My library", &library) == UMI_STATUS_OK);
    CHECK(UmiCreativeAssetLibraryInsert(library, "image", &asset) == UMI_STATUS_OK);
    CHECK(UmiRootedFileRemove(root, "private-source.bin") == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "existing") == 0)
        CHECK(UmiRootedFileWrite(root, leaf, "keep", 4U) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled-save") == 0)
        umi_cancellation_token_request(cancel);
    UmiCreativeAssetWriteResult receipt;
    UmiStatus status = UmiCreativeAssetLibrarySaveNew(
        library, strcmp(mode, "relative-save") == 0 ? "relative.umilibrary" : path, cancel, &receipt);
    if (strcmp(mode, "existing") == 0)
    {
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && !receipt.created);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(path, &size);
        CHECK(size == 4U && memcmp(bytes, "keep", size) == 0);
        free(bytes);
    }
    else if (strcmp(mode, "cancelled-save") == 0 || strcmp(mode, "relative-save") == 0)
    {
        CHECK(status ==
              (strcmp(mode, "cancelled-save") == 0 ? UMI_STATUS_CANCELLED : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(!receipt.created && !umi_fs_exists(path));
    }
    else
    {
        CHECK(status == UMI_STATUS_OK && receipt.created && receipt.file.closed);
        size_t size = 0U, encoded_size = 0U;
        unsigned char *bytes = FixtureRead(path, &size), *encoded = NULL;
        CHECK(UmiCreativeAssetLibraryArchiveEncode(library, NULL, &encoded, &encoded_size) == UMI_STATUS_OK);
        CHECK(size == encoded_size && memcmp(bytes, encoded, size) == 0 &&
              receipt.file.bytes_written == size);
        if (strcmp(mode, "source-privacy") == 0)
        {
            for (size_t i = 0U; i + strlen(source) <= size; ++i)
                CHECK(memcmp(bytes + i, source, strlen(source)) != 0);
        }
        if (strcmp(mode, "truncated") == 0)
            CHECK(UmiRootedFileWrite(root, leaf, bytes, 8U) == UMI_STATUS_OK);
        if (strcmp(mode, "corrupt") == 0)
        {
            bytes[size - 1U] ^= 1U;
            CHECK(UmiRootedFileWrite(root, leaf, bytes, size) == UMI_STATUS_OK);
        }
        free(bytes);
        UmiCreativeAssetLibraryArchiveFree(encoded);
        if (strcmp(mode, "cancelled-load") == 0)
            umi_cancellation_token_request(cancel);
        const char *selected = strcmp(mode, "relative-load") == 0 ? "relative.umilibrary"
                               : strcmp(mode, "missing") == 0     ? missing
                               : strcmp(mode, "directory") == 0   ? root
                                                                  : path;
        size_t limit = strcmp(mode, "payload-limit") == 0 ? 6U : 7U;
        if (strcmp(mode, "live-output") == 0)
        {
            UmiCreativeAssetLibrary *same = library;
            CHECK(UmiCreativeAssetLibraryLoad(path, 7U, NULL, &same) == UMI_STATUS_INVALID_STATE &&
                  same == library);
        }
        status = UmiCreativeAssetLibraryLoad(selected, limit, cancel, &loaded);
        if (strcmp(mode, "cancelled-load") == 0)
            CHECK(status == UMI_STATUS_CANCELLED && loaded == NULL);
        else if (strcmp(mode, "payload-limit") == 0)
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && loaded == NULL);
        else if (strcmp(mode, "truncated") == 0 || strcmp(mode, "corrupt") == 0)
            CHECK(status == UMI_STATUS_PARSE_ERROR && loaded == NULL);
        else if (strcmp(mode, "relative-load") == 0 || strcmp(mode, "missing") == 0 ||
                 strcmp(mode, "directory") == 0)
            CHECK(status != UMI_STATUS_OK && loaded == NULL);
        else
        {
            CHECK(status == UMI_STATUS_OK);
            UmiCreativeAssetLibraryEntry entry;
            CHECK(UmiCreativeAssetLibraryAt(loaded, 0U, &entry) == UMI_STATUS_OK &&
                  entry.asset.byte_count == 7U && entry.asset.source_path[0] == '\0');
        }
    }
    UmiCreativeAssetLibraryDestroy(library);
    UmiCreativeAssetLibraryDestroy(loaded);
    umi_cancellation_token_destroy(cancel);
    if (umi_fs_exists(path))
        CHECK(UmiRootedFileRemove(root, leaf) == UMI_STATUS_OK);
    /* Keep the isolated fixture directory as diagnostic evidence. */
    return 0;
}
