/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_form_descriptor.c
 *
 * PURPOSE:
 *   Validate describe a form, semantic root and Framework submit command.
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
#include "umicom/designer/visual_designer/form_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/form_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadFormDescriptorTransferEqual(const UmiRadFormDescriptor *a, const UmiRadFormDescriptor *b)
{
    return strcmp(a->form_id, b->form_id) == 0 &&
        strcmp(a->title, b->title) == 0 &&
        strcmp(a->root_component_id, b->root_component_id) == 0 &&
        strcmp(a->submit_command_id, b->submit_command_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadFormDescriptorTransferTails(UmiRadFormDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->form_id) + 1U;
        memset(value->form_id + used, 0xa5, sizeof(value->form_id) - used);
    }
    {
        size_t used = strlen(value->title) + 1U;
        memset(value->title + used, 0xa5, sizeof(value->title) - used);
    }
    {
        size_t used = strlen(value->root_component_id) + 1U;
        memset(value->root_component_id + used, 0xa5, sizeof(value->root_component_id) - used);
    }
    {
        size_t used = strlen(value->submit_command_id) + 1U;
        memset(value->submit_command_id + used, 0xa5, sizeof(value->submit_command_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadFormDescriptorTransferMalformed(const UmiRadFormDescriptor *sample)
{
    (void)sample;
    {
        UmiRadFormDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.form_id, 'x', sizeof(invalid.form_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_form_descriptor_is_valid(&invalid)) ||
            umi_rad_form_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated form_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadFormDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.title, 'x', sizeof(invalid.title));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_form_descriptor_is_valid(&invalid)) ||
            umi_rad_form_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated title was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadFormDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.root_component_id, 'x', sizeof(invalid.root_component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_form_descriptor_is_valid(&invalid)) ||
            umi_rad_form_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated root_component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadFormDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.submit_command_id, 'x', sizeof(invalid.submit_command_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_form_descriptor_is_valid(&invalid)) ||
            umi_rad_form_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated submit_command_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadFormDescriptorTransferCases, UmiRadFormDescriptor,
    umi_rad_form_descriptor_archive_encode, umi_rad_form_descriptor_archive_decode,
    UmiRadFormDescriptorTransferEqual, UmiRadFormDescriptorTransferTails, UmiRadFormDescriptorTransferMalformed)

int main(void){UmiRadFormDescriptor item;CHECK(umi_rad_form_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_form_descriptor_is_valid(&item));
    if (UmiRadFormDescriptorTransferCases(&item) != 0) return 1;
return 0;}
