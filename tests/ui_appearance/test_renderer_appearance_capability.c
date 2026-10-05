/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_appearance_capability.c
 *
 * PURPOSE:
 *   Verify declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
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
#include "umicom/ui/appearance/renderer_appearance_capability.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/renderer_appearance_capability.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceRendererAppearanceCapabilityTransferEqual(const UmiAppearanceRendererAppearanceCapability *a, const UmiAppearanceRendererAppearanceCapability *b)
{
    return strcmp(a->renderer_id, b->renderer_id) == 0 &&
        a->kind == b->kind &&
        a->supports_fractional_scale == b->supports_fractional_scale &&
        a->supports_high_contrast == b->supports_high_contrast &&
        a->supports_reduced_motion == b->supports_reduced_motion &&
        a->supports_symbolic_icons == b->supports_symbolic_icons;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceRendererAppearanceCapabilityTransferTails(UmiAppearanceRendererAppearanceCapability *value)
{
    (void)value;
    {
        size_t used = strlen(value->renderer_id) + 1U;
        memset(value->renderer_id + used, 0xa5, sizeof(value->renderer_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceRendererAppearanceCapabilityTransferMalformed(const UmiAppearanceRendererAppearanceCapability *sample)
{
    (void)sample;
    {
        UmiAppearanceRendererAppearanceCapability invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.renderer_id, 'x', sizeof(invalid.renderer_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_renderer_appearance_capability_is_valid(&invalid)) ||
            umi_appearance_renderer_appearance_capability_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated renderer_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceRendererAppearanceCapabilityTransferCases, UmiAppearanceRendererAppearanceCapability,
    umi_appearance_renderer_appearance_capability_archive_encode, umi_appearance_renderer_appearance_capability_archive_decode,
    UmiAppearanceRendererAppearanceCapabilityTransferEqual, UmiAppearanceRendererAppearanceCapabilityTransferTails, UmiAppearanceRendererAppearanceCapabilityTransferMalformed)

int main(void) {
    UmiAppearanceRendererAppearanceCapability item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_renderer_appearance_capability_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_renderer_appearance_capability_is_valid(&item)) return 2;
    if (UmiAppearanceRendererAppearanceCapabilityTransferCases(&item) != 0) return 1;

    return 0;
}
