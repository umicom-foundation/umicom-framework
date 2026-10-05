/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_event_descriptor.c
 *
 * PURPOSE:
 *   Validate describe an event exposed by a semantic component.
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
#include "umicom/designer/visual_designer/event_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/event_descriptor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadEventDescriptorTransferEqual(const UmiRadEventDescriptor *a, const UmiRadEventDescriptor *b)
{
    return strcmp(a->event_id, b->event_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->parameter_type, b->parameter_type) == 0 &&
        a->bindable == b->bindable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadEventDescriptorTransferTails(UmiRadEventDescriptor *value)
{
    (void)value;
    {
        size_t used = strlen(value->event_id) + 1U;
        memset(value->event_id + used, 0xa5, sizeof(value->event_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->parameter_type) + 1U;
        memset(value->parameter_type + used, 0xa5, sizeof(value->parameter_type) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadEventDescriptorTransferMalformed(const UmiRadEventDescriptor *sample)
{
    (void)sample;
    {
        UmiRadEventDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.event_id, 'x', sizeof(invalid.event_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_event_descriptor_is_valid(&invalid)) ||
            umi_rad_event_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated event_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadEventDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_event_descriptor_is_valid(&invalid)) ||
            umi_rad_event_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadEventDescriptor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parameter_type, 'x', sizeof(invalid.parameter_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_event_descriptor_is_valid(&invalid)) ||
            umi_rad_event_descriptor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parameter_type was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadEventDescriptorTransferCases, UmiRadEventDescriptor,
    umi_rad_event_descriptor_archive_encode, umi_rad_event_descriptor_archive_decode,
    UmiRadEventDescriptorTransferEqual, UmiRadEventDescriptorTransferTails, UmiRadEventDescriptorTransferMalformed)

int main(void){UmiRadEventDescriptor item;CHECK(umi_rad_event_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_event_descriptor_is_valid(&item));
    if (UmiRadEventDescriptorTransferCases(&item) != 0) return 1;
return 0;}
