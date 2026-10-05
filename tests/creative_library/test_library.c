/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_library/test_library.c
 * PURPOSE: Exercise collection ownership, stable identities, ordering and refusal without partial edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "check.h"
int main(int argc, char **argv)
{
    const char *cases[] = {"ownership",     "independent",   "duplicate",     "alias",
                           "invalid-id",    "empty-id",      "long-id",       "case-sensitive",
                           "missing",       "reorder",       "detach",        "output-live",
                           "title",         "unicode-title", "invalid-title", "count-limit",
                           "payload-limit", "exact-limit",   "empty-assets",  "invalid-arguments"};
    if (argc != 2 || !Known(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
        return 2;
    const char *mode = argv[1];
    UmiCreativeAssetLibrary *library = Library();
    UmiCreativeAssetLibraryInfo before = Info(library);
    CHECK(before.asset_count == 2U && before.byte_count == 8U);
    UmiCreativeAsset *asset = Capture("new"), *owned = asset;
    if (strcmp(mode, "ownership") == 0)
    {
        CHECK(UmiCreativeAssetLibraryInsert(library, "third", &asset) == UMI_STATUS_OK && asset == NULL);
        const UmiCreativeAsset *borrow = NULL;
        CHECK(UmiCreativeAssetLibraryBorrow(library, "third", &borrow) == UMI_STATUS_OK && borrow == owned);
        const void *bytes = NULL;
        size_t size = 0U;
        CHECK(UmiCreativeAssetBytes(borrow, &bytes, &size) == UMI_STATUS_OK && size == 4U);
        CHECK(((const unsigned char *)bytes)[1] == 255U);
    }
    else if (strcmp(mode, "independent") == 0)
    {
        UmiCreativeAssetLibrary *other = Library();
        CHECK(UmiCreativeAssetLibraryDetach(library, "opening", &owned) == UMI_STATUS_INVALID_STATE);
        UmiCreativeAsset *detached = NULL;
        CHECK(UmiCreativeAssetLibraryDetach(library, "opening", &detached) == UMI_STATUS_OK);
        UmiCreativeAssetDestroy(detached);
        CHECK(Info(other).asset_count == 2U && Info(library).asset_count == 1U);
        UmiCreativeAssetLibraryDestroy(other);
    }
    else if (strcmp(mode, "duplicate") == 0)
    {
        CHECK(UmiCreativeAssetLibraryInsert(library, "opening", &asset) == UMI_STATUS_ALREADY_EXISTS &&
              asset == owned);
        CHECK(Info(library).asset_count == 2U && Info(library).byte_count == 8U);
    }
    else if (strcmp(mode, "alias") == 0)
    {
        const UmiCreativeAsset *borrow = NULL;
        CHECK(UmiCreativeAssetLibraryBorrow(library, "opening", &borrow) == UMI_STATUS_OK);
        UmiCreativeAsset *alias = (UmiCreativeAsset *)borrow;
        CHECK(UmiCreativeAssetLibraryInsert(library, "alias", &alias) == UMI_STATUS_INVALID_STATE &&
              alias == borrow);
    }
    else if (strcmp(mode, "invalid-id") == 0 || strcmp(mode, "empty-id") == 0 || strcmp(mode, "long-id") == 0)
    {
        char long_id[UMI_CREATIVE_ID_CAPACITY + 1U];
        memset(long_id, 'a', sizeof(long_id) - 1U);
        long_id[sizeof(long_id) - 1U] = '\0';
        const char *id = strcmp(mode, "empty-id") == 0  ? ""
                         : strcmp(mode, "long-id") == 0 ? long_id
                                                        : "../opening";
        CHECK(UmiCreativeAssetLibraryInsert(library, id, &asset) == UMI_STATUS_INVALID_ARGUMENT &&
              asset == owned);
        CHECK(Info(library).asset_count == 2U);
    }
    else if (strcmp(mode, "case-sensitive") == 0)
    {
        CHECK(UmiCreativeAssetLibraryInsert(library, "Opening", &asset) == UMI_STATUS_OK);
        CHECK(Info(library).asset_count == 3U);
    }
    else if (strcmp(mode, "missing") == 0)
    {
        UmiCreativeAssetLibraryEntry output;
        memset(&output, 0x5A, sizeof(output));
        UmiCreativeAssetLibraryEntry previous = output;
        CHECK(UmiCreativeAssetLibraryAt(library, 99U, &output) == UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(&previous, &output, sizeof(output)) == 0);
        const UmiCreativeAsset *borrow = asset;
        CHECK(UmiCreativeAssetLibraryBorrow(library, "missing", &borrow) == UMI_STATUS_NOT_FOUND &&
              borrow == asset);
        CHECK(UmiCreativeAssetLibraryMove(library, "missing", 0U) == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(mode, "reorder") == 0)
    {
        CHECK(UmiCreativeAssetLibraryInsert(library, "third", &asset) == UMI_STATUS_OK);
        UmiCreativeAssetLibraryEntry entry;
        CHECK(UmiCreativeAssetLibraryMove(library, "opening", 2U) == UMI_STATUS_OK);
        CHECK(UmiCreativeAssetLibraryAt(library, 2U, &entry) == UMI_STATUS_OK &&
              strcmp(entry.id, "opening") == 0);
        CHECK(UmiCreativeAssetLibraryMove(library, "opening", 0U) == UMI_STATUS_OK);
        CHECK(UmiCreativeAssetLibraryAt(library, 0U, &entry) == UMI_STATUS_OK &&
              strcmp(entry.id, "opening") == 0);
        CHECK(UmiCreativeAssetLibraryMove(library, "opening", 0U) == UMI_STATUS_OK &&
              Info(library).byte_count == 12U);
        CHECK(UmiCreativeAssetLibraryMove(library, "opening", 3U) == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(mode, "detach") == 0)
    {
        UmiCreativeAsset *detached = NULL;
        CHECK(UmiCreativeAssetLibraryDetach(library, "opening", &detached) == UMI_STATUS_OK &&
              detached != NULL);
        CHECK(Info(library).asset_count == 1U && Info(library).byte_count == 4U);
        CHECK(UmiCreativeAssetLibraryInsert(library, "restored", &detached) == UMI_STATUS_OK &&
              detached == NULL);
        UmiCreativeAssetLibraryEntry entry;
        CHECK(UmiCreativeAssetLibraryAt(library, 0U, &entry) == UMI_STATUS_OK &&
              strcmp(entry.id, "closing") == 0);
    }
    else if (strcmp(mode, "output-live") == 0)
    {
        UmiCreativeAssetLibrary *same = library;
        CHECK(UmiCreativeAssetLibraryCreate("other", &same) == UMI_STATUS_INVALID_STATE && same == library);
        CHECK(UmiCreativeAssetLibraryDetach(library, "opening", &asset) == UMI_STATUS_INVALID_STATE &&
              asset == owned);
    }
    else if (strcmp(mode, "title") == 0 || strcmp(mode, "unicode-title") == 0 ||
             strcmp(mode, "invalid-title") == 0)
    {
        const char *title = strcmp(mode, "unicode-title") == 0   ? "caf\xc3\xa9"
                            : strcmp(mode, "invalid-title") == 0 ? "bad\nname"
                                                                 : "Updated title";
        UmiStatus status = UmiCreativeAssetLibraryTitle(library, title);
        CHECK(status == (strcmp(mode, "invalid-title") == 0 ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        CHECK(strcmp(Info(library).title, status == UMI_STATUS_OK ? title : before.title) == 0);
    }
    else if (strcmp(mode, "count-limit") == 0 || strcmp(mode, "empty-assets") == 0)
    {
        size_t initial = 2U;
        if (strcmp(mode, "empty-assets") == 0)
        {
            UmiCreativeAssetLibraryDestroy(library);
            library = NULL;
            CHECK(UmiCreativeAssetLibraryCreate("Empty captures", &library) == UMI_STATUS_OK);
            initial = 0U;
        }
        for (size_t i = initial; i < UMI_CREATIVE_LIBRARY_MAX_ASSETS; ++i)
        {
            char id[32];
            (void)snprintf(id, sizeof(id), "asset-%zu", i);
            UmiCreativeAsset *empty = NULL;
            CHECK(UmiCreativeAssetCapture("empty", UMI_CREATIVE_ASSET_BINARY, NULL, 0U, NULL, &empty) ==
                  UMI_STATUS_OK);
            CHECK(UmiCreativeAssetLibraryInsert(library, id, &empty) == UMI_STATUS_OK);
        }
        CHECK(Info(library).asset_count == 32U && Info(library).byte_count == initial * 4U);
        CHECK(UmiCreativeAssetLibraryInsert(library, "overflow", &asset) == UMI_STATUS_CAPACITY_EXCEEDED &&
              asset == owned);
    }
    else if (strcmp(mode, "payload-limit") == 0 || strcmp(mode, "exact-limit") == 0)
    {
        size_t count = UMI_CREATIVE_LIBRARY_MAX_BYTES - (strcmp(mode, "exact-limit") == 0 ? 8U : 7U);
        unsigned char *bytes = calloc(count, 1U);
        CHECK(bytes != NULL);
        UmiCreativeAsset *large = NULL;
        CHECK(UmiCreativeAssetCapture("large", UMI_CREATIVE_ASSET_BINARY, bytes, count, NULL, &large) ==
              UMI_STATUS_OK);
        free(bytes);
        UmiStatus status = UmiCreativeAssetLibraryInsert(library, "large", &large);
        if (strcmp(mode, "exact-limit") == 0)
            CHECK(status == UMI_STATUS_OK && large == NULL &&
                  Info(library).byte_count == UMI_CREATIVE_LIBRARY_MAX_BYTES);
        else
            CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && large != NULL && Info(library).byte_count == 8U);
        UmiCreativeAssetDestroy(large);
    }
    else
    {
        CHECK(UmiCreativeAssetLibraryCreate(NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetLibraryInspect(NULL, &before) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetLibraryInsert(NULL, "asset", &asset) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetLibraryDetach(NULL, "asset", &asset) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetLibraryBorrow(NULL, "asset", NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCreativeAssetLibraryMove(NULL, "asset", 0U) == UMI_STATUS_INVALID_ARGUMENT);
        UmiCreativeAssetLibraryDestroy(NULL);
    }
    UmiCreativeAssetDestroy(asset);
    UmiCreativeAssetLibraryDestroy(library);
    return 0;
}
