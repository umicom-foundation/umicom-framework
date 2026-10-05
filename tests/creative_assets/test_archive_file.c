/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_archive_file.c
 * PURPOSE: Exercise durable asset archives, payload bounds and refusal paths with isolated native files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/creative_workspace/asset_archive.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"roundtrip",      "empty",          "unicode-path",   "existing",
                           "cancelled-save", "cancelled-load", "relative-save",  "relative-load",
                           "missing-load",   "directory-load", "truncated-load", "corrupted-load",
                           "payload-limit",  "source-removed", "live-output",    "invalid-limit",
                           "chunked",        "source-privacy"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(cases[i], mode) == 0);
    if (known != 1U)
        return 2;
    char root[UMI_PATH_CAPACITY], source[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY],
        missing[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    FixturePath(source, root, "source.bin");
    FixturePath(missing, root, "missing.umiasset");
    const char *leaf = strcmp(mode, "unicode-path") == 0 ? "caf\xc3\xa9.umiasset" : "capture.umiasset";
    FixturePath(path, root, leaf);
    size_t count = strcmp(mode, "chunked") == 0 ? 150000U : strcmp(mode, "empty") == 0 ? 0U : 4U;
    unsigned char *payload = malloc(count + 1U);
    CHECK(payload != NULL);
    for (size_t i = 0U; i < count; ++i)
        payload[i] = (unsigned char)(i % 251U);
    CHECK(UmiRootedFileWrite(root, "source.bin", payload, count) == UMI_STATUS_OK);
    UmiCreativeAsset *asset = NULL, *restored = NULL;
    CHECK(UmiCreativeAssetLoadFile(source, "local asset", UMI_CREATIVE_ASSET_VIDEO, count, NULL, &asset) ==
          UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "existing") == 0)
        CHECK(UmiRootedFileWrite(root, leaf, "keep", 4U) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled-save") == 0)
        umi_cancellation_token_request(cancel);
    UmiCreativeAssetWriteResult receipt;
    UmiStatus status = UmiCreativeAssetArchiveSaveNew(
        asset, strcmp(mode, "relative-save") == 0 ? "capture.umiasset" : path, cancel, &receipt);
    if (strcmp(mode, "existing") == 0)
    {
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && !receipt.created);
        size_t actual_size = 0U;
        unsigned char *actual = FixtureRead(path, &actual_size);
        CHECK(actual_size == 4U && memcmp(actual, "keep", 4U) == 0);
        free(actual);
    }
    else if (strcmp(mode, "cancelled-save") == 0)
    {
        CHECK(status == UMI_STATUS_CANCELLED && !receipt.created && !umi_fs_exists(path));
    }
    else if (strcmp(mode, "relative-save") == 0)
    {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && !receipt.created && !umi_fs_exists(path));
    }
    else
    {
        CHECK(status == UMI_STATUS_OK && receipt.created && receipt.file.closed);
        unsigned char *encoded = NULL;
        size_t encoded_size = 0U;
        CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, &encoded, &encoded_size) == UMI_STATUS_OK);
        size_t actual_size = 0U;
        unsigned char *actual = FixtureRead(path, &actual_size);
        CHECK(actual_size == encoded_size && memcmp(actual, encoded, actual_size) == 0 &&
              receipt.file.bytes_written == actual_size);
        if (strcmp(mode, "source-privacy") == 0)
        {
            /* The exact schema comparison above already excludes extra fields.
             * Check the captured source name is absent from this small fixture. */
            bool found = false;
            size_t source_size = strlen(source);
            for (size_t i = 0U; i + source_size <= actual_size; ++i)
                if (memcmp(actual + i, source, source_size) == 0)
                    found = true;
            CHECK(!found);
        }
        free(actual);
        UmiCreativeAssetArchiveFree(encoded);
        if (strcmp(mode, "source-removed") == 0)
            CHECK(UmiRootedFileRemove(root, "source.bin") == UMI_STATUS_OK);
        if (strcmp(mode, "truncated-load") == 0)
            CHECK(UmiRootedFileWrite(root, leaf, "UMIASSET", 8U) == UMI_STATUS_OK);
        if (strcmp(mode, "corrupted-load") == 0)
        {
            unsigned char *bad = NULL;
            size_t bad_size = 0U;
            CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, &bad, &bad_size) == UMI_STATUS_OK);
            bad[bad_size - 1U] ^= 1U;
            CHECK(UmiRootedFileWrite(root, leaf, bad, bad_size) == UMI_STATUS_OK);
            UmiCreativeAssetArchiveFree(bad);
        }
        if (strcmp(mode, "cancelled-load") == 0)
            umi_cancellation_token_request(cancel);
        const char *selected = strcmp(mode, "relative-load") == 0    ? "capture.umiasset"
                               : strcmp(mode, "missing-load") == 0   ? missing
                               : strcmp(mode, "directory-load") == 0 ? root
                                                                     : path;
        size_t limit = strcmp(mode, "payload-limit") == 0   ? count - 1U
                       : strcmp(mode, "invalid-limit") == 0 ? UMI_CREATIVE_ASSET_MAX_BYTES + 1U
                                                            : count;
        if (strcmp(mode, "live-output") == 0)
        {
            UmiCreativeAsset *same = asset;
            CHECK(UmiCreativeAssetArchiveLoad(path, count, NULL, &same) == UMI_STATUS_INVALID_STATE &&
                  same == asset);
        }
        status = UmiCreativeAssetArchiveLoad(selected, limit, cancel, &restored);
        if (strcmp(mode, "cancelled-load") == 0)
            CHECK(status == UMI_STATUS_CANCELLED && restored == NULL);
        else if (strcmp(mode, "payload-limit") == 0 || strcmp(mode, "invalid-limit") == 0)
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && restored == NULL);
        else if (strcmp(mode, "truncated-load") == 0 || strcmp(mode, "corrupted-load") == 0)
            CHECK(status == UMI_STATUS_PARSE_ERROR && restored == NULL);
        else if (strcmp(mode, "relative-load") == 0 || strcmp(mode, "missing-load") == 0 ||
                 strcmp(mode, "directory-load") == 0)
            CHECK(status != UMI_STATUS_OK && restored == NULL);
        else
        {
            CHECK(status == UMI_STATUS_OK && restored != NULL);
            UmiCreativeAssetDestroy(asset);
            asset = NULL;
            UmiCreativeAssetInfo info;
            const void *view = NULL;
            size_t restored_size = 0U;
            CHECK(UmiCreativeAssetInspect(restored, &info) == UMI_STATUS_OK && info.source_path[0] == '\0' &&
                  info.declared_kind == UMI_CREATIVE_ASSET_VIDEO && strcmp(info.label, "local asset") == 0);
            CHECK(UmiCreativeAssetBytes(restored, &view, &restored_size) == UMI_STATUS_OK &&
                  restored_size == count && memcmp(view, payload, count) == 0);
        }
    }
    UmiCreativeAssetDestroy(asset);
    UmiCreativeAssetDestroy(restored);
    umi_cancellation_token_destroy(cancel);
    free(payload);
    return 0;
}
