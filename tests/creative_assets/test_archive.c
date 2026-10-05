/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_archive.c
 * PURPOSE: Exercise portable archive compatibility, immutable ownership, strict malformed-input handling and cancellation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/asset_archive.h"
#include "umicom/native_launcher/sha256.h"
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
static const unsigned char fixture[] = {
    0x55U, 0x4dU, 0x49U, 0x41U, 0x53U, 0x53U, 0x45U, 0x54U, 0x01U, 0x00U, 0x06U, 0x00U, 0x04U,
    0x00U, 0x00U, 0x00U, 0x03U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x67U, 0x6fU,
    0x6cU, 0x64U, 0x00U, 0xffU, 0x0aU, 0x82U, 0xabU, 0xf8U, 0x0cU, 0xd4U, 0x99U, 0xe7U, 0x53U,
    0x72U, 0xdaU, 0x58U, 0xe3U, 0xa3U, 0x72U, 0x29U, 0x3fU, 0x35U, 0x6cU, 0x7dU, 0x7cU, 0x4dU,
    0x5bU, 0xf3U, 0x70U, 0x88U, 0xdfU, 0xd3U, 0xcdU, 0xf4U, 0x7eU, 0xbfU, 0x5cU};
/* This fixture was calculated independently of the C encoder. A format change
 * must account for readable existing archives, not merely make both sides agree. */
static void Seal(unsigned char *bytes, size_t size)
{
    UmiNativeSha256 hash;
    UmiNativeSha256Init(&hash);
    if (UmiNativeSha256Update(&hash, bytes, size - 32U) != UMI_STATUS_OK ||
        UmiNativeSha256Final(&hash, bytes + size - 32U) != UMI_STATUS_OK)
        abort();
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *cases[] = {"roundtrip",        "binary",           "empty",
                                            "purposes",         "unicode-label",    "longest-label",
                                            "chunked",          "deterministic",    "fixture",
                                            "truncated",        "trailing",         "magic",
                                            "schema",           "reserved",         "kind",
                                            "empty-label",      "long-label",       "nul-label",
                                            "utf8-label",       "control-label",    "oversized-payload",
                                            "wrong-size",       "corrupt-payload",  "corrupt-digest",
                                            "cancelled-encode", "cancelled-decode", "live-output",
                                            "invalid-arguments"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(cases[i], mode) == 0);
    if (known != 1U)
        return 2;
    UmiCreativeAsset *asset = NULL, *restored = NULL;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    size_t payload_size = strcmp(mode, "chunked") == 0 ? 150000U : strcmp(mode, "empty") == 0 ? 0U : 3U;
    unsigned char *payload = malloc(payload_size + 1U);
    CHECK(payload != NULL);
    for (size_t i = 0U; i < payload_size; ++i)
        payload[i] = (unsigned char)(i % 251U);
    if (payload_size == 3U)
    {
        payload[0] = 0U;
        payload[1] = 255U;
        payload[2] = 10U;
    }
    char label[UMI_CREATIVE_LABEL_CAPACITY] = "gold";
    if (strcmp(mode, "unicode-label") == 0)
        memcpy(label, "caf\xc3\xa9", 6U);
    if (strcmp(mode, "longest-label") == 0)
    {
        memset(label, 'x', sizeof(label) - 1U);
        label[sizeof(label) - 1U] = '\0';
    }
    CHECK(UmiCreativeAssetCapture(label, UMI_CREATIVE_ASSET_BINARY, payload, payload_size, NULL, &asset) ==
          UMI_STATUS_OK);
    unsigned char *wire = NULL;
    size_t size = 999U;
    CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, &wire, &size) == UMI_STATUS_OK);
    CHECK(size == 24U + strlen(label) + payload_size + 32U);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "fixture") == 0)
        CHECK(size == sizeof(fixture) && memcmp(wire, fixture, sizeof(fixture)) == 0);
    if (strcmp(mode, "deterministic") == 0)
    {
        unsigned char *second = NULL;
        size_t second_size = 0U;
        CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, &second, &second_size) == UMI_STATUS_OK);
        CHECK(second_size == size && memcmp(second, wire, size) == 0);
        UmiCreativeAssetArchiveFree(second);
    }
    if (strcmp(mode, "purposes") == 0)
    {
        for (int kind = UMI_CREATIVE_ASSET_DOCUMENT; kind <= UMI_CREATIVE_ASSET_BINARY; ++kind)
        {
            UmiCreativeAsset *typed = NULL, *decoded = NULL;
            unsigned char *encoded = NULL;
            size_t encoded_size = 0U;
            CHECK(UmiCreativeAssetCapture("purpose", (UmiCreativeAssetKind)kind, payload, payload_size, NULL,
                                          &typed) == UMI_STATUS_OK);
            CHECK(UmiCreativeAssetArchiveEncode(typed, NULL, &encoded, &encoded_size) == UMI_STATUS_OK);
            CHECK(UmiCreativeAssetArchiveDecode(encoded, encoded_size, NULL, &decoded) == UMI_STATUS_OK);
            UmiCreativeAssetInfo info;
            CHECK(UmiCreativeAssetInspect(decoded, &info) == UMI_STATUS_OK &&
                  info.declared_kind == (UmiCreativeAssetKind)kind);
            UmiCreativeAssetDestroy(typed);
            UmiCreativeAssetDestroy(decoded);
            UmiCreativeAssetArchiveFree(encoded);
        }
    }
    if (strcmp(mode, "truncated") == 0)
    {
        for (size_t truncated = 0U; truncated < size; ++truncated)
        {
            CHECK(UmiCreativeAssetArchiveDecode(wire, truncated, NULL, &restored) == UMI_STATUS_PARSE_ERROR);
            CHECK(restored == NULL);
        }
    }
    if (strcmp(mode, "trailing") == 0)
    {
        unsigned char *extended = malloc(size + 1U);
        CHECK(extended != NULL);
        memcpy(extended, wire, size);
        extended[size] = 0U;
        CHECK(UmiCreativeAssetArchiveDecode(extended, size + 1U, NULL, &restored) == UMI_STATUS_PARSE_ERROR &&
              restored == NULL);
        free(extended);
    }
    if (strcmp(mode, "reserved") == 0)
    {
        const size_t offsets[] = {11U, 14U, 15U};
        for (size_t i = 0U; i < 3U; ++i)
        {
            wire[offsets[i]] = 1U;
            Seal(wire, size);
            CHECK(UmiCreativeAssetArchiveDecode(wire, size, NULL, &restored) == UMI_STATUS_PARSE_ERROR &&
                  restored == NULL);
            wire[offsets[i]] = 0U;
        }
        Seal(wire, size);
    }
    if (strcmp(mode, "magic") == 0)
    {
        wire[0] ^= 1U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "schema") == 0)
    {
        wire[8] = 2U;
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "kind") == 0)
    {
        wire[10] = 0U;
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "empty-label") == 0)
    {
        wire[12] = 0U;
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "long-label") == 0)
    {
        wire[12] = 255U;
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "nul-label") == 0)
    {
        wire[25] = 0U;
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "utf8-label") == 0)
    {
        wire[25] = 255U;
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "control-label") == 0)
    {
        wire[25] = '\n';
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "oversized-payload") == 0)
    {
        memset(wire + 16U, 255, 8U);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "wrong-size") == 0)
    {
        wire[16] = 2U;
        Seal(wire, size);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "corrupt-payload") == 0)
    {
        wire[28] ^= 1U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "corrupt-digest") == 0)
    {
        wire[size - 1U] ^= 1U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "cancelled-encode") == 0)
    {
        unsigned char *other = NULL;
        size_t count = 999U;
        umi_cancellation_token_request(cancel);
        CHECK(UmiCreativeAssetArchiveEncode(asset, cancel, &other, &count) == UMI_STATUS_CANCELLED &&
              other == NULL && count == 999U);
        umi_cancellation_token_reset(cancel);
    }
    if (strcmp(mode, "cancelled-decode") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "live-output") == 0)
    {
        unsigned char *same = wire;
        size_t count = 777U;
        CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, &same, &count) == UMI_STATUS_INVALID_STATE &&
              same == wire && count == 777U);
        UmiCreativeAsset *original = asset;
        CHECK(UmiCreativeAssetArchiveDecode(wire, size, NULL, &original) == UMI_STATUS_INVALID_STATE &&
              original == asset);
    }
    if (strcmp(mode, "invalid-arguments") == 0)
    {
        unsigned char *other = NULL;
        size_t count = 123U;
        CHECK(UmiCreativeAssetArchiveEncode(NULL, NULL, &other, &count) == UMI_STATUS_INVALID_ARGUMENT &&
              other == NULL && count == 123U);
        CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, NULL, &count) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetArchiveEncode(asset, NULL, &other, NULL) == UMI_STATUS_INVALID_ARGUMENT &&
              other == NULL);
        CHECK(UmiCreativeAssetArchiveDecode(NULL, size, NULL, &restored) == UMI_STATUS_INVALID_ARGUMENT &&
              restored == NULL);
        CHECK(UmiCreativeAssetArchiveDecode(wire, size, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetArchiveDecode(wire, UMI_CREATIVE_ASSET_ARCHIVE_MAX_BYTES + 1U, NULL,
                                            &restored) == UMI_STATUS_CAPACITY_EXCEEDED &&
              restored == NULL);
        UmiCreativeAssetArchiveFree(NULL);
    }
    CHECK(UmiCreativeAssetArchiveDecode(wire, size, cancel, &restored) == expected);
    if (expected == UMI_STATUS_OK)
    {
        /* Destroy source owners before inspecting the decoded capture: it must
         * own every byte, rather than retain views into the input envelope. */
        UmiCreativeAssetDestroy(asset);
        asset = NULL;
        UmiCreativeAssetArchiveFree(wire);
        wire = NULL;
        UmiCreativeAssetInfo info;
        const void *view = NULL;
        size_t count = 0U;
        CHECK(UmiCreativeAssetInspect(restored, &info) == UMI_STATUS_OK);
        CHECK(strcmp(info.label, label) == 0 && info.declared_kind == UMI_CREATIVE_ASSET_BINARY &&
              info.source_path[0] == '\0');
        CHECK(UmiCreativeAssetBytes(restored, &view, &count) == UMI_STATUS_OK && count == payload_size &&
              memcmp(view, payload, count) == 0);
    }
    else
        CHECK(restored == NULL);
    UmiCreativeAssetArchiveFree(wire);
    UmiCreativeAssetDestroy(asset);
    UmiCreativeAssetDestroy(restored);
    umi_cancellation_token_destroy(cancel);
    free(payload);
    return 0;
}
