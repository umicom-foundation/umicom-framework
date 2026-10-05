/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_property_binding.c
 *
 * PURPOSE:
 *   Validate describe a visual property binding backed by the canonical reactive UI state layer.
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
#include "umicom/designer/visual_designer/property_binding.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/property_binding.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPropertyBindingTransferEqual(const UmiRadPropertyBinding *a, const UmiRadPropertyBinding *b)
{
    return strcmp(a->binding_id, b->binding_id) == 0 &&
        strcmp(a->source_path, b->source_path) == 0 &&
        strcmp(a->target_path, b->target_path) == 0 &&
        a->two_way == b->two_way &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPropertyBindingTransferTails(UmiRadPropertyBinding *value)
{
    (void)value;
    {
        size_t used = strlen(value->binding_id) + 1U;
        memset(value->binding_id + used, 0xa5, sizeof(value->binding_id) - used);
    }
    {
        size_t used = strlen(value->source_path) + 1U;
        memset(value->source_path + used, 0xa5, sizeof(value->source_path) - used);
    }
    {
        size_t used = strlen(value->target_path) + 1U;
        memset(value->target_path + used, 0xa5, sizeof(value->target_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPropertyBindingTransferMalformed(const UmiRadPropertyBinding *sample)
{
    (void)sample;
    {
        UmiRadPropertyBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.binding_id, 'x', sizeof(invalid.binding_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_binding_is_valid(&invalid)) ||
            umi_rad_property_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated binding_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_path, 'x', sizeof(invalid.source_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_binding_is_valid(&invalid)) ||
            umi_rad_property_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadPropertyBinding invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_path, 'x', sizeof(invalid.target_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_property_binding_is_valid(&invalid)) ||
            umi_rad_property_binding_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPropertyBindingTransferCases, UmiRadPropertyBinding,
    umi_rad_property_binding_archive_encode, umi_rad_property_binding_archive_decode,
    UmiRadPropertyBindingTransferEqual, UmiRadPropertyBindingTransferTails, UmiRadPropertyBindingTransferMalformed)

int main(void){UmiRadPropertyBinding item;CHECK(umi_rad_property_binding_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_property_binding_is_valid(&item));
    if (UmiRadPropertyBindingTransferCases(&item) != 0) return 1;
return 0;}
