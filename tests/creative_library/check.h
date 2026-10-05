/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_library/check.h
 * PURPOSE: Provide explicit assertions and small owned fixtures for creative library regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_CREATIVE_LIBRARY_CHECK_H
#define UMICOM_TEST_CREATIVE_LIBRARY_CHECK_H
#include "umicom/creative_workspace/asset_library_archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression);                                 \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
/* Fixtures use binary bytes, including NUL, so a string-based implementation
 * cannot accidentally satisfy complete-content ownership checks. */
static inline UmiCreativeAsset *Capture(const char *label)
{
    const unsigned char bytes[] = {0U, 255U, 65U, 128U};
    UmiCreativeAsset *asset = NULL;
    CHECK(UmiCreativeAssetCapture(label, UMI_CREATIVE_ASSET_IMAGE, bytes, sizeof(bytes), NULL, &asset) ==
          UMI_STATUS_OK);
    return asset;
}
static inline UmiCreativeAssetLibrary *Library(void)
{
    UmiCreativeAssetLibrary *library = NULL;
    CHECK(UmiCreativeAssetLibraryCreate("Tutorial assets", &library) == UMI_STATUS_OK);
    UmiCreativeAsset *first = Capture("Opening image"), *second = Capture("caf\xc3\xa9");
    CHECK(UmiCreativeAssetLibraryInsert(library, "opening", &first) == UMI_STATUS_OK && first == NULL);
    CHECK(UmiCreativeAssetLibraryInsert(library, "closing", &second) == UMI_STATUS_OK && second == NULL);
    return library;
}
static inline UmiCreativeAssetLibraryInfo Info(const UmiCreativeAssetLibrary *library)
{
    UmiCreativeAssetLibraryInfo info;
    CHECK(UmiCreativeAssetLibraryInspect(library, &info) == UMI_STATUS_OK);
    return info;
}
static inline int Known(const char *mode, const char *const *cases, size_t count)
{
    for (size_t i = 0U; i < count; ++i)
        if (strcmp(mode, cases[i]) == 0)
            return 1;
    return 0;
}
#endif
