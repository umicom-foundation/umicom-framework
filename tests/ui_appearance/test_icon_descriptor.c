/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_descriptor.c
 *
 * PURPOSE:
 *   Verify describe a semantic icon identity, directionality and scalable/symbolic capabilities.
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
#include "umicom/ui/appearance/icon_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/icon_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceIconDescriptorTransferEqual(const UmiAppearanceIconDescriptor *a, const UmiAppearanceIconDescriptor *b)
{
    return strcmp(a->icon_id, b->icon_id) == 0 &&
        strcmp(a->semantic_role, b->semantic_role) == 0 &&
        a->scalable == b->scalable &&
        a->symbolic == b->symbolic &&
        a->direction_sensitive == b->direction_sensitive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceIconDescriptorTransferTails(UmiAppearanceIconDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->icon_id) + 1U;
        memset(value->icon_id + used, 0xa5, sizeof(value->icon_id) - used);
    }
    {
        size_t used = strlen(value->semantic_role) + 1U;
        memset(value->semantic_role + used, 0xa5, sizeof(value->semantic_role) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceIconDescriptorTransferMalformed(const UmiAppearanceIconDescriptor *sample)
{
    (void)sample;
    {
        UmiAppearanceIconDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.icon_id, 'x', sizeof(invalid.icon_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_icon_descriptor_is_valid(&invalid)) ||
            umi_appearance_icon_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated icon_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceIconDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.semantic_role, 'x', sizeof(invalid.semantic_role));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_icon_descriptor_is_valid(&invalid)) ||
            umi_appearance_icon_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated semantic_role was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceIconDescriptorTransferCases, UmiAppearanceIconDescriptor,
    umi_appearance_icon_descriptor_archive_encode, umi_appearance_icon_descriptor_archive_decode,
    UmiAppearanceIconDescriptorTransferEqual, UmiAppearanceIconDescriptorTransferTails, UmiAppearanceIconDescriptorTransferMalformed)

int main(void) {
    UmiAppearanceIconDescriptor item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_icon_descriptor_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_icon_descriptor_is_valid(&item)) return 2;
    if (UmiAppearanceIconDescriptorTransferCases(&item) != 0) return 1;

    return 0;
}
