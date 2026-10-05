/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_property_editor_descriptor.c
 *
 * PURPOSE:
 *   Validate describe an editor choice for a semantic component property.
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
#include "umicom/designer/visual_designer/property_editor_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/property_editor_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPropertyEditorDescriptorTransferEqual(const UmiRadPropertyEditorDescriptor *a, const UmiRadPropertyEditorDescriptor *b)
{
    return strcmp(a->property_id, b->property_id) == 0 &&
        strcmp(a->editor_type, b->editor_type) == 0 &&
        strcmp(a->value_type, b->value_type) == 0 &&
        a->required == b->required &&
        a->read_only == b->read_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPropertyEditorDescriptorTransferTails(UmiRadPropertyEditorDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->property_id) + 1U;
        memset(value->property_id + used, 0xa5, sizeof(value->property_id) - used);
    }
    {
        size_t used = strlen(value->editor_type) + 1U;
        memset(value->editor_type + used, 0xa5, sizeof(value->editor_type) - used);
    }
    {
        size_t used = strlen(value->value_type) + 1U;
        memset(value->value_type + used, 0xa5, sizeof(value->value_type) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPropertyEditorDescriptorTransferMalformed(const UmiRadPropertyEditorDescriptor *sample)
{
    (void)sample;
    {
        UmiRadPropertyEditorDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_id, 'x', sizeof(invalid.property_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_editor_descriptor_is_valid(&invalid)) ||
            umi_rad_property_editor_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyEditorDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.editor_type, 'x', sizeof(invalid.editor_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_editor_descriptor_is_valid(&invalid)) ||
            umi_rad_property_editor_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated editor_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyEditorDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value_type, 'x', sizeof(invalid.value_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_editor_descriptor_is_valid(&invalid)) ||
            umi_rad_property_editor_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value_type was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPropertyEditorDescriptorTransferCases, UmiRadPropertyEditorDescriptor,
    umi_rad_property_editor_descriptor_archive_encode, umi_rad_property_editor_descriptor_archive_decode,
    UmiRadPropertyEditorDescriptorTransferEqual, UmiRadPropertyEditorDescriptorTransferTails, UmiRadPropertyEditorDescriptorTransferMalformed)

int main(void){UmiRadPropertyEditorDescriptor item;CHECK(umi_rad_property_editor_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_editor_descriptor_is_valid(&item));
    if (UmiRadPropertyEditorDescriptorTransferCases(&item) != 0) return 1;
return 0;}
