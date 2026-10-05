/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_asset.c
 * PURPOSE: Check complete asset ownership, purpose metadata, input bounds and cancellation without opening files or providers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/asset.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"binary",        "empty",           "owned-bytes",    "owned-label",
                           "unicode",       "all-kinds",       "invalid-kind",   "empty-label",
                           "control-label", "invalid-utf8",    "label-capacity", "null-bytes",
                           "null-output",   "live-output",     "too-large",      "cancel",
                           "cancel-reset",  "inspect-invalid", "view-invalid",   "two-assets",
                           "large-capture"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    unsigned char source[] = {0U, 255U, 10U, 0U, 128U};
    const unsigned char expected[] = {0U, 255U, 10U, 0U, 128U};
    char label[UMI_CREATIVE_LABEL_CAPACITY] = "Captured item";
    UmiCreativeAsset *asset = NULL;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "unicode") == 0)
        strcpy(label, "caf\xc3\xa9 \xd9\x85\xd9\x84\xd9\x81");
    UmiStatus failure = UMI_STATUS_OK;
    const void *input = source;
    size_t size = sizeof(source);
    UmiCreativeAssetKind kind = UMI_CREATIVE_ASSET_IMAGE;
    if (strcmp(mode, "empty") == 0)
    {
        size = 0U;
        input = NULL;
    }
    if (strcmp(mode, "invalid-kind") == 0)
    {
        kind = (UmiCreativeAssetKind)99;
        failure = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "empty-label") == 0)
    {
        label[0] = '\0';
        failure = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "control-label") == 0)
    {
        strcpy(label, "bad\nlabel");
        failure = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        strcpy(label, "bad\xc0\x80");
        failure = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "label-capacity") == 0)
    {
        memset(label, 'x', sizeof(label));
        failure = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "null-bytes") == 0)
    {
        input = NULL;
        failure = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "too-large") == 0)
    {
        size = (size_t)UMI_CREATIVE_ASSET_MAX_BYTES + 1U;
        failure = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "cancel") == 0 || strcmp(mode, "cancel-reset") == 0)
    {
        umi_cancellation_token_request(cancel);
        failure = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "cancel-reset") == 0)
    {
        umi_cancellation_token_reset(cancel);
        failure = UMI_STATUS_OK;
    }
    if (strcmp(mode, "null-output") == 0)
        CHECK(UmiCreativeAssetCapture(label, kind, input, size, cancel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    unsigned char *large = NULL;
    if (strcmp(mode, "large-capture") == 0)
    {
        size = 150000U;
        large = malloc(size);
        CHECK(large != NULL);
        for (size_t i = 0U; i < size; ++i)
            large[i] = (unsigned char)(i % 251U);
        input = large;
    }
    CHECK(UmiCreativeAssetCapture(label, kind, input, size, cancel, &asset) == failure);
    if (failure != UMI_STATUS_OK)
    {
        CHECK(asset == NULL);
        umi_cancellation_token_destroy(cancel);
        free(large);
        return 0;
    }
    if (strcmp(mode, "owned-bytes") == 0)
        memset(source, 0x77, sizeof(source));
    if (strcmp(mode, "owned-label") == 0)
        label[0] = '!';
    UmiCreativeAssetInfo info;
    CHECK(UmiCreativeAssetInspect(asset, &info) == UMI_STATUS_OK);
    CHECK(info.byte_count == size && info.source_path[0] == '\0' && info.declared_kind == kind);
    if (strcmp(mode, "owned-label") == 0)
        CHECK(strcmp(info.label, "Captured item") == 0);
    const void *view = NULL;
    size_t viewed = 0U;
    CHECK(UmiCreativeAssetBytes(asset, &view, &viewed) == UMI_STATUS_OK);
    CHECK(view != NULL && viewed == size);
    if (size != 0U)
        CHECK(memcmp(view, large != NULL ? large : expected, size) == 0);
    if (strcmp(mode, "live-output") == 0)
    {
        UmiCreativeAsset *same = asset;
        CHECK(UmiCreativeAssetCapture("new", kind, input, size, cancel, &asset) == UMI_STATUS_INVALID_STATE);
        CHECK(asset == same);
    }
    if (strcmp(mode, "inspect-invalid") == 0)
    {
        unsigned char before[sizeof(info)];
        memcpy(before, &info, sizeof(info));
        CHECK(UmiCreativeAssetInspect(NULL, &info) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before, &info, sizeof(info)) == 0);
        CHECK(UmiCreativeAssetInspect(asset, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (strcmp(mode, "view-invalid") == 0)
    {
        CHECK(UmiCreativeAssetBytes(NULL, &view, &viewed) == UMI_STATUS_INVALID_ARGUMENT && view == NULL &&
              viewed == 0U);
        CHECK(UmiCreativeAssetBytes(asset, NULL, &viewed) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetBytes(asset, &view, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    if (strcmp(mode, "all-kinds") == 0)
    {
        for (int item = UMI_CREATIVE_ASSET_DOCUMENT; item <= UMI_CREATIVE_ASSET_BINARY; ++item)
        {
            UmiCreativeAsset *other = NULL;
            CHECK(UmiCreativeAssetCapture("other", (UmiCreativeAssetKind)item, "different", 9U, NULL,
                                          &other) == UMI_STATUS_OK);
            CHECK(UmiCreativeAssetInspect(other, &info) == UMI_STATUS_OK &&
                  info.declared_kind == (UmiCreativeAssetKind)item);
            UmiCreativeAssetDestroy(other);
        }
        CHECK(UmiCreativeAssetBytes(asset, &view, &viewed) == UMI_STATUS_OK && viewed == size &&
              memcmp(view, expected, size) == 0);
    }
    if (strcmp(mode, "two-assets") == 0)
    {
        UmiCreativeAsset *other = NULL;
        CHECK(UmiCreativeAssetCapture("other", UMI_CREATIVE_ASSET_LYRICS, "second", 6U, NULL, &other) ==
              UMI_STATUS_OK);
        UmiCreativeAssetDestroy(asset);
        asset = NULL;
        CHECK(UmiCreativeAssetBytes(other, &view, &viewed) == UMI_STATUS_OK);
        CHECK(viewed == 6U && memcmp(view, "second", 6U) == 0);
        UmiCreativeAssetDestroy(other);
    }
    UmiCreativeAssetDestroy(asset);
    UmiCreativeAssetDestroy(NULL);
    umi_cancellation_token_destroy(cancel);
    free(large);
    return 0;
}
