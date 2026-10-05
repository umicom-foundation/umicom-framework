/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_surface_semantics.c
 *
 * PURPOSE:
 *   Verify describe semantic surface hierarchy and elevation intent independently of renderer primitives.
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
#include "umicom/ui/appearance/surface_semantics.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/surface_semantics.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceSurfaceSemanticsTransferEqual(const UmiAppearanceSurfaceSemantics *a, const UmiAppearanceSurfaceSemantics *b)
{
    return strcmp(a->surface_id, b->surface_id) == 0 &&
        strcmp(a->background_role, b->background_role) == 0 &&
        strcmp(a->foreground_role, b->foreground_role) == 0 &&
        a->elevation_level == b->elevation_level;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceSurfaceSemanticsTransferTails(UmiAppearanceSurfaceSemantics *value)
{
    (void)value;
    {
        size_t used = strlen(value->surface_id) + 1U;
        memset(value->surface_id + used, 0xa5, sizeof(value->surface_id) - used);
    }
    {
        size_t used = strlen(value->background_role) + 1U;
        memset(value->background_role + used, 0xa5, sizeof(value->background_role) - used);
    }
    {
        size_t used = strlen(value->foreground_role) + 1U;
        memset(value->foreground_role + used, 0xa5, sizeof(value->foreground_role) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceSurfaceSemanticsTransferMalformed(const UmiAppearanceSurfaceSemantics *sample)
{
    (void)sample;
    {
        UmiAppearanceSurfaceSemantics invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.surface_id, 'x', sizeof(invalid.surface_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_surface_semantics_is_valid(&invalid)) ||
            umi_appearance_surface_semantics_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated surface_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceSurfaceSemantics invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.background_role, 'x', sizeof(invalid.background_role));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_surface_semantics_is_valid(&invalid)) ||
            umi_appearance_surface_semantics_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated background_role was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceSurfaceSemantics invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.foreground_role, 'x', sizeof(invalid.foreground_role));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_surface_semantics_is_valid(&invalid)) ||
            umi_appearance_surface_semantics_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated foreground_role was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceSurfaceSemanticsTransferCases, UmiAppearanceSurfaceSemantics,
    umi_appearance_surface_semantics_archive_encode, umi_appearance_surface_semantics_archive_decode,
    UmiAppearanceSurfaceSemanticsTransferEqual, UmiAppearanceSurfaceSemanticsTransferTails, UmiAppearanceSurfaceSemanticsTransferMalformed)

int main(void) {
    UmiAppearanceSurfaceSemantics item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_surface_semantics_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_surface_semantics_is_valid(&item)) return 2;
    if (UmiAppearanceSurfaceSemanticsTransferCases(&item) != 0) return 1;

    return 0;
}
