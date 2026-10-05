/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_asset_file.c
 * PURPOSE: Check selected local file snapshots and new-file copying without replacing source files or existing destinations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/creative_workspace/asset.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"load",          "unicode-path", "empty",       "limit",           "oversized",
                           "missing",       "directory",    "relative",    "changed-source",  "copy",
                           "copy-existing", "cancel-load",  "cancel-copy", "bad-destination", "live-output",
                           "chunked-copy"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    char root[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], destination[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    const char *leaf = strcmp(mode, "unicode-path") == 0 ? "caf\xc3\xa9.bin" : "source.bin";
    FixturePath(source, root, leaf);
    FixturePath(destination, root, "copied.bin");
    size_t size = strcmp(mode, "chunked-copy") == 0 ? 150000U : strcmp(mode, "empty") == 0 ? 0U : 5U;
    unsigned char *bytes = malloc(size + 1U);
    CHECK(bytes != NULL);
    for (size_t i = 0U; i < size; ++i)
        bytes[i] = (unsigned char)(i % 251U);
    CHECK(UmiRootedFileWrite(root, leaf, bytes, size) == UMI_STATUS_OK);
    UmiCreativeAsset *asset = NULL;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiStatus status;
    const char *selected = source;
    size_t bound = size;
    if (strcmp(mode, "oversized") == 0)
        bound = size - 1U;
    if (strcmp(mode, "missing") == 0)
        selected = destination;
    if (strcmp(mode, "directory") == 0)
        selected = root;
    if (strcmp(mode, "relative") == 0)
        selected = "source.bin";
    if (strcmp(mode, "cancel-load") == 0)
        umi_cancellation_token_request(cancel);
    status = UmiCreativeAssetLoadFile(selected, "Source", UMI_CREATIVE_ASSET_BINARY, bound, cancel, &asset);
    if (strcmp(mode, "oversized") == 0)
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && asset == NULL);
    else if (strcmp(mode, "cancel-load") == 0)
        CHECK(status == UMI_STATUS_CANCELLED && asset == NULL);
    else if (strcmp(mode, "missing") == 0 || strcmp(mode, "directory") == 0 || strcmp(mode, "relative") == 0)
        CHECK(status != UMI_STATUS_OK && asset == NULL);
    else
    {
        CHECK(status == UMI_STATUS_OK && asset != NULL);
        UmiCreativeAssetInfo info;
        CHECK(UmiCreativeAssetInspect(asset, &info) == UMI_STATUS_OK && info.byte_count == size &&
              strcmp(info.source_path, source) == 0);
        if (strcmp(mode, "live-output") == 0)
        {
            UmiCreativeAsset *same = asset;
            CHECK(UmiCreativeAssetLoadFile(source, "again", UMI_CREATIVE_ASSET_BINARY, bound, cancel,
                                           &asset) == UMI_STATUS_INVALID_STATE &&
                  same == asset);
        }
        if (strcmp(mode, "changed-source") == 0)
            CHECK(UmiRootedFileWrite(root, leaf, "replacement", 11U) == UMI_STATUS_OK);
        const void *view = NULL;
        size_t count = 0U;
        CHECK(UmiCreativeAssetBytes(asset, &view, &count) == UMI_STATUS_OK && count == size &&
              memcmp(view, bytes, size) == 0);
        if (strcmp(mode, "copy-existing") == 0)
            CHECK(UmiRootedFileWrite(root, "copied.bin", "keep", 4U) == UMI_STATUS_OK);
        if (strcmp(mode, "cancel-copy") == 0)
            umi_cancellation_token_request(cancel);
        UmiCreativeAssetWriteResult report;
        status = UmiCreativeAssetWriteNew(
            asset, strcmp(mode, "bad-destination") == 0 ? "relative.bin" : destination, cancel, &report);
        if (strcmp(mode, "copy-existing") == 0)
        {
            CHECK(status == UMI_STATUS_ALREADY_EXISTS && !report.created);
            size_t actual_size = 0U;
            unsigned char *actual = FixtureRead(destination, &actual_size);
            CHECK(actual_size == 4U && memcmp(actual, "keep", 4U) == 0);
            free(actual);
        }
        else if (strcmp(mode, "cancel-copy") == 0)
            CHECK(status == UMI_STATUS_CANCELLED && !report.created && !umi_fs_exists(destination));
        else if (strcmp(mode, "bad-destination") == 0)
            CHECK(status == UMI_STATUS_INVALID_ARGUMENT && !report.created && !umi_fs_exists(destination));
        else
        {
            CHECK(status == UMI_STATUS_OK && report.created && report.file.closed &&
                  report.file.bytes_written == size);
            size_t actual_size = 0U;
            unsigned char *actual = FixtureRead(destination, &actual_size);
            CHECK(actual_size == size && memcmp(actual, bytes, size) == 0);
            free(actual);
        }
    }
    UmiCreativeAssetDestroy(asset);
    umi_cancellation_token_destroy(cancel);
    free(bytes);
    return 0;
}
