/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_elevation_style_projection.c
 *
 * PURPOSE:
 *   Verify resolve semantic elevation levels to shadow and border tokens suitable for each frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/appearance/elevation_style_projection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/elevation_style_projection.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceElevationStyleProjectionTransferEqual(const UmiAppearanceElevationStyleProjection *a, const UmiAppearanceElevationStyleProjection *b)
{
    return strcmp(a->style_id, b->style_id) == 0 &&
        a->elevation_level == b->elevation_level &&
        strcmp(a->shadow_token, b->shadow_token) == 0 &&
        strcmp(a->fallback_border_token, b->fallback_border_token) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceElevationStyleProjectionTransferTails(UmiAppearanceElevationStyleProjection *value)
{
    (void)value;
    {
        size_t used = strlen(value->style_id) + 1U;
        memset(value->style_id + used, 0xa5, sizeof(value->style_id) - used);
    }
    {
        size_t used = strlen(value->shadow_token) + 1U;
        memset(value->shadow_token + used, 0xa5, sizeof(value->shadow_token) - used);
    }
    {
        size_t used = strlen(value->fallback_border_token) + 1U;
        memset(value->fallback_border_token + used, 0xa5, sizeof(value->fallback_border_token) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceElevationStyleProjectionTransferMalformed(const UmiAppearanceElevationStyleProjection *sample)
{
    (void)sample;
    {
        UmiAppearanceElevationStyleProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.style_id, 'x', sizeof(invalid.style_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_elevation_style_projection_is_valid(&invalid)) ||
            umi_appearance_elevation_style_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated style_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceElevationStyleProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.shadow_token, 'x', sizeof(invalid.shadow_token));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_elevation_style_projection_is_valid(&invalid)) ||
            umi_appearance_elevation_style_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated shadow_token was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceElevationStyleProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.fallback_border_token, 'x', sizeof(invalid.fallback_border_token));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_elevation_style_projection_is_valid(&invalid)) ||
            umi_appearance_elevation_style_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated fallback_border_token was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceElevationStyleProjectionTransferCases, UmiAppearanceElevationStyleProjection,
    umi_appearance_elevation_style_projection_archive_encode, umi_appearance_elevation_style_projection_archive_decode,
    UmiAppearanceElevationStyleProjectionTransferEqual, UmiAppearanceElevationStyleProjectionTransferTails, UmiAppearanceElevationStyleProjectionTransferMalformed)

int main(void) {
    UmiAppearanceElevationStyleProjection item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_elevation_style_projection_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_elevation_style_projection_is_valid(&item)) return 2;
    if (UmiAppearanceElevationStyleProjectionTransferCases(&item) != 0) return 1;

    return 0;
}
