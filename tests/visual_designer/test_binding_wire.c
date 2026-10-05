/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_binding_wire.c
 *
 * PURPOSE:
 *   Validate represent a directed visual binding wire between endpoints.
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
#include "umicom/designer/visual_designer/binding_wire.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/binding_wire.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadBindingWireTransferEqual(const UmiRadBindingWire *a, const UmiRadBindingWire *b)
{
    return strcmp(a->wire_id, b->wire_id) == 0 &&
        strcmp(a->source_node_id, b->source_node_id) == 0 &&
        strcmp(a->target_node_id, b->target_node_id) == 0 &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadBindingWireTransferTails(UmiRadBindingWire *value)
{
    (void)value;
    {
        size_t used = strlen(value->wire_id) + 1U;
        memset(value->wire_id + used, 0xa5, sizeof(value->wire_id) - used);
    }
    {
        size_t used = strlen(value->source_node_id) + 1U;
        memset(value->source_node_id + used, 0xa5, sizeof(value->source_node_id) - used);
    }
    {
        size_t used = strlen(value->target_node_id) + 1U;
        memset(value->target_node_id + used, 0xa5, sizeof(value->target_node_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadBindingWireTransferMalformed(const UmiRadBindingWire *sample)
{
    (void)sample;
    {
        UmiRadBindingWire invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.wire_id, 'x', sizeof(invalid.wire_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_binding_wire_is_valid(&invalid)) ||
            umi_rad_binding_wire_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated wire_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadBindingWire invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_node_id, 'x', sizeof(invalid.source_node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_binding_wire_is_valid(&invalid)) ||
            umi_rad_binding_wire_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_node_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadBindingWire invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_node_id, 'x', sizeof(invalid.target_node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_binding_wire_is_valid(&invalid)) ||
            umi_rad_binding_wire_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_node_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadBindingWireTransferCases, UmiRadBindingWire,
    umi_rad_binding_wire_archive_encode, umi_rad_binding_wire_archive_decode,
    UmiRadBindingWireTransferEqual, UmiRadBindingWireTransferTails, UmiRadBindingWireTransferMalformed)

int main(void){UmiRadBindingWire item;CHECK(umi_rad_binding_wire_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_binding_wire_is_valid(&item));
    if (UmiRadBindingWireTransferCases(&item) != 0) return 1;
return 0;}
