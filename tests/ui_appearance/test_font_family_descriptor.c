/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_font_family_descriptor.c
 *
 * PURPOSE:
 *   Verify describe one semantic font-family candidate and its broad typographic classification.
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
#include "umicom/ui/appearance/font_family_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/font_family_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceFontFamilyDescriptorTransferEqual(const UmiAppearanceFontFamilyDescriptor *a, const UmiAppearanceFontFamilyDescriptor *b)
{
    return strcmp(a->family_id, b->family_id) == 0 &&
        strcmp(a->family_name, b->family_name) == 0 &&
        strcmp(a->classification, b->classification) == 0 &&
        a->monospace == b->monospace &&
        a->variable_font == b->variable_font;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceFontFamilyDescriptorTransferTails(UmiAppearanceFontFamilyDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->family_id) + 1U;
        memset(value->family_id + used, 0xa5, sizeof(value->family_id) - used);
    }
    {
        size_t used = strlen(value->family_name) + 1U;
        memset(value->family_name + used, 0xa5, sizeof(value->family_name) - used);
    }
    {
        size_t used = strlen(value->classification) + 1U;
        memset(value->classification + used, 0xa5, sizeof(value->classification) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceFontFamilyDescriptorTransferMalformed(const UmiAppearanceFontFamilyDescriptor *sample)
{
    (void)sample;
    {
        UmiAppearanceFontFamilyDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.family_id, 'x', sizeof(invalid.family_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_font_family_descriptor_is_valid(&invalid)) ||
            umi_appearance_font_family_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated family_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceFontFamilyDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.family_name, 'x', sizeof(invalid.family_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_font_family_descriptor_is_valid(&invalid)) ||
            umi_appearance_font_family_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated family_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceFontFamilyDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.classification, 'x', sizeof(invalid.classification));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_font_family_descriptor_is_valid(&invalid)) ||
            umi_appearance_font_family_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated classification was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceFontFamilyDescriptorTransferCases, UmiAppearanceFontFamilyDescriptor,
    umi_appearance_font_family_descriptor_archive_encode, umi_appearance_font_family_descriptor_archive_decode,
    UmiAppearanceFontFamilyDescriptorTransferEqual, UmiAppearanceFontFamilyDescriptorTransferTails, UmiAppearanceFontFamilyDescriptorTransferMalformed)

int main(void) {
    UmiAppearanceFontFamilyDescriptor item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_font_family_descriptor_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_font_family_descriptor_is_valid(&item)) return 2;
    if (UmiAppearanceFontFamilyDescriptorTransferCases(&item) != 0) return 1;

    return 0;
}
