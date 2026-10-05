/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_library/test_archive.c
 * PURPOSE: Validate portable library bytes against independent framing and malformed collection cases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "check.h"
#include "umicom/native_launcher/sha256.h"
static void Digest(unsigned char *bytes, size_t size)
{
    UmiNativeSha256 hash;
    UmiNativeSha256Init(&hash);
    CHECK(UmiNativeSha256Update(&hash, bytes, size - 32U) == UMI_STATUS_OK);
    CHECK(UmiNativeSha256Final(&hash, bytes + size - 32U) == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"roundtrip",
                           "empty-library",
                           "empty-payload",
                           "purposes",
                           "deterministic",
                           "independent-fixture",
                           "unicode-title",
                           "longest-fields",
                           "chunked",
                           "cancelled-encode",
                           "cancelled-decode",
                           "live-output",
                           "invalid-arguments",
                           "invalid-limit",
                           "payload-limit",
                           "magic",
                           "schema",
                           "reserved",
                           "count",
                           "title-size",
                           "title-nul",
                           "title-utf8",
                           "total-size",
                           "record-reserved",
                           "id-size",
                           "id-nul",
                           "id-invalid",
                           "duplicate-id",
                           "inner-size",
                           "inner-digest",
                           "outer-digest",
                           "trailing",
                           "truncated",
                           "record-count"};
    if (argc != 2 || !Known(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
        return 2;
    const char *mode = argv[1];
    UmiCreativeAssetLibrary *library = Library(), *decoded = NULL;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "empty-library") == 0 || strcmp(mode, "independent-fixture") == 0 ||
        strcmp(mode, "empty-payload") == 0)
    {
        UmiCreativeAssetLibraryDestroy(library);
        library = NULL;
        CHECK(UmiCreativeAssetLibraryCreate("A", &library) == UMI_STATUS_OK);
        if (strcmp(mode, "empty-payload") == 0)
        {
            UmiCreativeAsset *empty = NULL;
            CHECK(UmiCreativeAssetCapture("empty", UMI_CREATIVE_ASSET_DOCUMENT, NULL, 0U, NULL, &empty) ==
                  UMI_STATUS_OK);
            CHECK(UmiCreativeAssetLibraryInsert(library, "empty", &empty) == UMI_STATUS_OK);
        }
    }
    if (strcmp(mode, "unicode-title") == 0)
        CHECK(UmiCreativeAssetLibraryTitle(library, "\xe6\x97\xa5\xe6\x9c\xac") == UMI_STATUS_OK);
    if (strcmp(mode, "longest-fields") == 0)
    {
        char title[192], id[64];
        memset(title, 'T', 191U);
        title[191] = '\0';
        memset(id, 'a', 63U);
        id[63] = '\0';
        CHECK(UmiCreativeAssetLibraryTitle(library, title) == UMI_STATUS_OK);
        UmiCreativeAsset *asset = Capture(title);
        CHECK(UmiCreativeAssetLibraryInsert(library, id, &asset) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "purposes") == 0)
    {
        for (unsigned i = 1U; i <= 6U; ++i)
        {
            UmiCreativeAsset *asset = NULL;
            char id[16];
            (void)snprintf(id, sizeof(id), "purpose-%u", i);
            CHECK(UmiCreativeAssetCapture(id, (UmiCreativeAssetKind)i, "bytes", 5U, NULL, &asset) ==
                  UMI_STATUS_OK);
            CHECK(UmiCreativeAssetLibraryInsert(library, id, &asset) == UMI_STATUS_OK);
        }
    }
    if (strcmp(mode, "chunked") == 0)
    {
        size_t count = 150000U;
        unsigned char *payload = malloc(count);
        CHECK(payload != NULL);
        for (size_t i = 0U; i < count; ++i)
            payload[i] = (unsigned char)(i % 251U);
        UmiCreativeAsset *asset = NULL;
        CHECK(UmiCreativeAssetCapture("large", UMI_CREATIVE_ASSET_VIDEO, payload, count, NULL, &asset) ==
              UMI_STATUS_OK);
        CHECK(UmiCreativeAssetLibraryInsert(library, "large", &asset) == UMI_STATUS_OK);
        free(payload);
    }
    unsigned char *bytes = NULL;
    size_t size = 99U;
    if (strcmp(mode, "cancelled-encode") == 0)
    {
        umi_cancellation_token_request(cancel);
        CHECK(UmiCreativeAssetLibraryArchiveEncode(library, cancel, &bytes, &size) == UMI_STATUS_CANCELLED &&
              bytes == NULL && size == 99U);
    }
    else
    {
        CHECK(UmiCreativeAssetLibraryArchiveEncode(library, NULL, &bytes, &size) == UMI_STATUS_OK);
        UmiCreativeAssetLibraryInfo info = Info(library);
        size_t cursor = 32U + strlen(info.title), first_archive = cursor + 16U + 7U;
        UmiStatus expected = UMI_STATUS_OK;
        size_t limit = UMI_CREATIVE_LIBRARY_MAX_BYTES;
        if (strcmp(mode, "independent-fixture") == 0)
        {
            /* This fixture is assembled from the storage specification, without
             * invoking any asset or library serializer to produce its fields. */
            unsigned char fixture[65] = {0};
            memcpy(fixture, "UMILIBRY", 8U);
            fixture[8] = 1U;
            fixture[16] = 1U;
            fixture[32] = 'A';
            Digest(fixture, sizeof(fixture));
            CHECK(size == sizeof(fixture) && memcmp(bytes, fixture, size) == 0);
        }
        else if (strcmp(mode, "deterministic") == 0)
        {
            unsigned char *again = NULL;
            size_t again_size = 0U;
            CHECK(UmiCreativeAssetLibraryArchiveEncode(library, NULL, &again, &again_size) == UMI_STATUS_OK);
            CHECK(again_size == size && memcmp(bytes, again, size) == 0);
            UmiCreativeAssetLibraryArchiveFree(again);
        }
        else if (strcmp(mode, "live-output") == 0)
        {
            UmiCreativeAssetLibrary *same = library;
            CHECK(UmiCreativeAssetLibraryArchiveDecode(bytes, size, limit, NULL, &same) ==
                      UMI_STATUS_INVALID_STATE &&
                  same == library);
            unsigned char *same_bytes = bytes;
            size_t previous = size;
            CHECK(UmiCreativeAssetLibraryArchiveEncode(library, NULL, &same_bytes, &previous) ==
                      UMI_STATUS_INVALID_STATE &&
                  same_bytes == bytes && previous == size);
        }
        else if (strcmp(mode, "invalid-arguments") == 0)
        {
            CHECK(UmiCreativeAssetLibraryArchiveDecode(NULL, size, limit, NULL, &decoded) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiCreativeAssetLibraryArchiveDecode(bytes, size, limit, NULL, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiCreativeAssetLibraryArchiveEncode(NULL, NULL, NULL, &size) ==
                  UMI_STATUS_INVALID_ARGUMENT);
        }
        else if (strcmp(mode, "cancelled-decode") == 0)
        {
            umi_cancellation_token_request(cancel);
            expected = UMI_STATUS_CANCELLED;
        }
        else if (strcmp(mode, "invalid-limit") == 0)
        {
            limit = UMI_CREATIVE_LIBRARY_MAX_BYTES + 1U;
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (strcmp(mode, "payload-limit") == 0)
        {
            limit = 7U;
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (strcmp(mode, "magic") == 0)
        {
            bytes[0] ^= 1U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "schema") == 0)
        {
            bytes[8] = 2U;
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        }
        else if (strcmp(mode, "reserved") == 0)
        {
            bytes[18] = 1U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "count") == 0)
        {
            bytes[12] = 33U;
            expected = UMI_STATUS_CAPACITY_EXCEEDED;
        }
        else if (strcmp(mode, "title-size") == 0)
        {
            bytes[16] = 0U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "title-nul") == 0)
        {
            bytes[33] = 0U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "title-utf8") == 0)
        {
            bytes[33] = 255U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "total-size") == 0)
        {
            bytes[24] = 9U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "record-reserved") == 0)
        {
            bytes[cursor + 10U] = 1U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "id-size") == 0)
        {
            bytes[cursor + 8U] = 64U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "id-nul") == 0)
        {
            bytes[cursor + 17U] = 0U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "id-invalid") == 0)
        {
            bytes[cursor + 16U] = '/';
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "duplicate-id") == 0)
        {
            size_t archive_size = (size_t)bytes[cursor];
            size_t second_id = first_archive + archive_size + 16U;
            memcpy(bytes + second_id, "opening", 7U);
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "inner-size") == 0)
        {
            bytes[cursor] = 255U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "inner-digest") == 0)
        {
            bytes[first_archive + 30U] ^= 1U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "outer-digest") == 0)
        {
            bytes[size - 1U] ^= 1U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "trailing") == 0)
        {
            unsigned char *larger = realloc(bytes, size + 1U);
            CHECK(larger != NULL);
            bytes = larger;
            bytes[size] = 0U;
            ++size;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "truncated") == 0)
        {
            --size;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        else if (strcmp(mode, "record-count") == 0)
        {
            bytes[12] = 1U;
            expected = UMI_STATUS_PARSE_ERROR;
        }
        if (expected == UMI_STATUS_PARSE_ERROR && strcmp(mode, "outer-digest") != 0 &&
            strcmp(mode, "truncated") != 0)
            Digest(bytes, size);
        CHECK(UmiCreativeAssetLibraryArchiveDecode(bytes, size, limit, cancel, &decoded) == expected);
        if (expected == UMI_STATUS_OK)
        {
            CHECK(Info(decoded).asset_count == info.asset_count &&
                  Info(decoded).byte_count == info.byte_count &&
                  strcmp(Info(decoded).title, info.title) == 0);
            for (size_t i = 0U; i < info.asset_count; ++i)
            {
                UmiCreativeAssetLibraryEntry before, after;
                CHECK(UmiCreativeAssetLibraryAt(library, i, &before) == UMI_STATUS_OK);
                CHECK(UmiCreativeAssetLibraryAt(decoded, i, &after) == UMI_STATUS_OK);
                CHECK(strcmp(before.id, after.id) == 0 &&
                      strcmp(before.asset.label, after.asset.label) == 0 &&
                      before.asset.declared_kind == after.asset.declared_kind &&
                      after.asset.source_path[0] == '\0');
                const UmiCreativeAsset *a = NULL, *b = NULL;
                const void *a_bytes = NULL, *b_bytes = NULL;
                size_t a_size = 0U, b_size = 0U;
                CHECK(UmiCreativeAssetLibraryBorrow(library, before.id, &a) == UMI_STATUS_OK);
                CHECK(UmiCreativeAssetLibraryBorrow(decoded, after.id, &b) == UMI_STATUS_OK);
                CHECK(UmiCreativeAssetBytes(a, &a_bytes, &a_size) == UMI_STATUS_OK &&
                      UmiCreativeAssetBytes(b, &b_bytes, &b_size) == UMI_STATUS_OK);
                CHECK(a_size == b_size && memcmp(a_bytes, b_bytes, a_size) == 0);
            }
        }
        else
            CHECK(decoded == NULL);
    }
    UmiCreativeAssetLibraryArchiveFree(bytes);
    UmiCreativeAssetLibraryDestroy(decoded);
    UmiCreativeAssetLibraryDestroy(library);
    umi_cancellation_token_destroy(cancel);
    return 0;
}
