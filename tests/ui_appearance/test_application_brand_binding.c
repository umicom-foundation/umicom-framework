/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_application_brand_binding.c
 *
 * PURPOSE:
 *   Verify bind a thin application identity to Framework-owned brand and theme-pack identifiers.
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
#include "umicom/ui/appearance/application_brand_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/application_brand_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceApplicationBrandBindingTransferEqual(const UmiAppearanceApplicationBrandBinding *a, const UmiAppearanceApplicationBrandBinding *b)
{
    return strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->brand_id, b->brand_id) == 0 &&
        strcmp(a->theme_pack_id, b->theme_pack_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceApplicationBrandBindingTransferTails(UmiAppearanceApplicationBrandBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->brand_id) + 1U;
        memset(value->brand_id + used, 0xa5, sizeof(value->brand_id) - used);
    }
    {
        size_t used = strlen(value->theme_pack_id) + 1U;
        memset(value->theme_pack_id + used, 0xa5, sizeof(value->theme_pack_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceApplicationBrandBindingTransferMalformed(const UmiAppearanceApplicationBrandBinding *sample)
{
    (void)sample;
    {
        UmiAppearanceApplicationBrandBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_application_brand_binding_is_valid(&invalid)) ||
            umi_appearance_application_brand_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceApplicationBrandBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.brand_id, 'x', sizeof(invalid.brand_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_application_brand_binding_is_valid(&invalid)) ||
            umi_appearance_application_brand_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated brand_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceApplicationBrandBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.theme_pack_id, 'x', sizeof(invalid.theme_pack_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_application_brand_binding_is_valid(&invalid)) ||
            umi_appearance_application_brand_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated theme_pack_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceApplicationBrandBindingTransferCases, UmiAppearanceApplicationBrandBinding,
    umi_appearance_application_brand_binding_archive_encode, umi_appearance_application_brand_binding_archive_decode,
    UmiAppearanceApplicationBrandBindingTransferEqual, UmiAppearanceApplicationBrandBindingTransferTails, UmiAppearanceApplicationBrandBindingTransferMalformed)

int main(void) {
    UmiAppearanceApplicationBrandBinding item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_application_brand_binding_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_application_brand_binding_is_valid(&item)) return 2;
    if (UmiAppearanceApplicationBrandBindingTransferCases(&item) != 0) return 1;

    return 0;
}
