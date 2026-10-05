/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_component_style_binding.c
 *
 * PURPOSE:
 *   Verify bind a semantic component and state map to a Framework style identity.
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
#include "umicom/ui/appearance/component_style_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/component_style_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceComponentStyleBindingTransferEqual(const UmiAppearanceComponentStyleBinding *a, const UmiAppearanceComponentStyleBinding *b)
{
    return strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->style_id, b->style_id) == 0 &&
        strcmp(a->state_map_id, b->state_map_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceComponentStyleBindingTransferTails(UmiAppearanceComponentStyleBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->style_id) + 1U;
        memset(value->style_id + used, 0xa5, sizeof(value->style_id) - used);
    }
    {
        size_t used = strlen(value->state_map_id) + 1U;
        memset(value->state_map_id + used, 0xa5, sizeof(value->state_map_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceComponentStyleBindingTransferMalformed(const UmiAppearanceComponentStyleBinding *sample)
{
    (void)sample;
    {
        UmiAppearanceComponentStyleBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_component_style_binding_is_valid(&invalid)) ||
            umi_appearance_component_style_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceComponentStyleBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.style_id, 'x', sizeof(invalid.style_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_component_style_binding_is_valid(&invalid)) ||
            umi_appearance_component_style_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated style_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceComponentStyleBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.state_map_id, 'x', sizeof(invalid.state_map_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_component_style_binding_is_valid(&invalid)) ||
            umi_appearance_component_style_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated state_map_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceComponentStyleBindingTransferCases, UmiAppearanceComponentStyleBinding,
    umi_appearance_component_style_binding_archive_encode, umi_appearance_component_style_binding_archive_decode,
    UmiAppearanceComponentStyleBindingTransferEqual, UmiAppearanceComponentStyleBindingTransferTails, UmiAppearanceComponentStyleBindingTransferMalformed)

int main(void) {
    UmiAppearanceComponentStyleBinding item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_component_style_binding_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_component_style_binding_is_valid(&item)) return 2;
    if (UmiAppearanceComponentStyleBindingTransferCases(&item) != 0) return 1;

    return 0;
}
