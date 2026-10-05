/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_binding_endpoint.c
 *
 * PURPOSE:
 *   Validate represent one source or destination property endpoint in the visual binding editor.
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
#include "umicom/designer/visual_designer/binding_endpoint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/binding_endpoint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadBindingEndpointTransferEqual(const UmiRadBindingEndpoint *a, const UmiRadBindingEndpoint *b)
{
    return strcmp(a->node_id, b->node_id) == 0 &&
        strcmp(a->property_path, b->property_path) == 0 &&
        a->output == b->output;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadBindingEndpointTransferTails(UmiRadBindingEndpoint *value)
{
    (void)value;
    {
        size_t used = strlen(value->node_id) + 1U;
        memset(value->node_id + used, 0xa5, sizeof(value->node_id) - used);
    }
    {
        size_t used = strlen(value->property_path) + 1U;
        memset(value->property_path + used, 0xa5, sizeof(value->property_path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadBindingEndpointTransferMalformed(const UmiRadBindingEndpoint *sample)
{
    (void)sample;
    {
        UmiRadBindingEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.node_id, 'x', sizeof(invalid.node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_binding_endpoint_is_valid(&invalid)) ||
            umi_rad_binding_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated node_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadBindingEndpoint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_path, 'x', sizeof(invalid.property_path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_binding_endpoint_is_valid(&invalid)) ||
            umi_rad_binding_endpoint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadBindingEndpointTransferCases, UmiRadBindingEndpoint,
    umi_rad_binding_endpoint_archive_encode, umi_rad_binding_endpoint_archive_decode,
    UmiRadBindingEndpointTransferEqual, UmiRadBindingEndpointTransferTails, UmiRadBindingEndpointTransferMalformed)

int main(void){UmiRadBindingEndpoint item;CHECK(umi_rad_binding_endpoint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_binding_endpoint_is_valid(&item));
    if (UmiRadBindingEndpointTransferCases(&item) != 0) return 1;
return 0;}
