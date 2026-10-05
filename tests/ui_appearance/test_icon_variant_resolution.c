/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_variant_resolution.c
 *
 * PURPOSE:
 *   Verify resolve light/dark/high-contrast and direction-aware icon variants while preserving semantic identity.
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
#include "umicom/ui/appearance/icon_variant_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/icon_variant_resolution.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceIconVariantResolutionTransferEqual(const UmiAppearanceIconVariantResolution *a, const UmiAppearanceIconVariantResolution *b)
{
    return strcmp(a->icon_id, b->icon_id) == 0 &&
        strcmp(a->resolved_variant_id, b->resolved_variant_id) == 0 &&
        a->mode == b->mode &&
        a->rtl == b->rtl &&
        a->mirrored == b->mirrored;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceIconVariantResolutionTransferTails(UmiAppearanceIconVariantResolution *value)
{
    (void)value;
    {
        size_t used = strlen(value->icon_id) + 1U;
        memset(value->icon_id + used, 0xa5, sizeof(value->icon_id) - used);
    }
    {
        size_t used = strlen(value->resolved_variant_id) + 1U;
        memset(value->resolved_variant_id + used, 0xa5, sizeof(value->resolved_variant_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceIconVariantResolutionTransferMalformed(const UmiAppearanceIconVariantResolution *sample)
{
    (void)sample;
    {
        UmiAppearanceIconVariantResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.icon_id, 'x', sizeof(invalid.icon_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_icon_variant_resolution_is_valid(&invalid)) ||
            umi_appearance_icon_variant_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated icon_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceIconVariantResolution invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resolved_variant_id, 'x', sizeof(invalid.resolved_variant_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_icon_variant_resolution_is_valid(&invalid)) ||
            umi_appearance_icon_variant_resolution_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resolved_variant_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceIconVariantResolutionTransferCases, UmiAppearanceIconVariantResolution,
    umi_appearance_icon_variant_resolution_archive_encode, umi_appearance_icon_variant_resolution_archive_decode,
    UmiAppearanceIconVariantResolutionTransferEqual, UmiAppearanceIconVariantResolutionTransferTails, UmiAppearanceIconVariantResolutionTransferMalformed)

int main(void) {
    UmiAppearanceIconVariantResolution item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_icon_variant_resolution_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_icon_variant_resolution_is_valid(&item)) return 2;
    if (UmiAppearanceIconVariantResolutionTransferCases(&item) != 0) return 1;

    umi_appearance_icon_variant_resolution_set_direction(&item,1,1); /* Apply this branch only when its contract condition is satisfied. */ if(!item.mirrored) return 3;
    return 0;
}
