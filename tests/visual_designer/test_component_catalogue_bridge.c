/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_component_catalogue_bridge.c
 *
 * PURPOSE:
 *   Validate map Design System component identifiers to canonical designer component types.
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
#include "umicom/designer/visual_designer/component_catalogue_bridge.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/component_catalogue_bridge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadComponentCatalogueBridgeTransferEqual(const UmiRadComponentCatalogueBridge *a, const UmiRadComponentCatalogueBridge *b)
{
    return strcmp(a->design_component_id, b->design_component_id) == 0 &&
        strcmp(a->designer_type, b->designer_type) == 0 &&
        strcmp(a->family, b->family) == 0 &&
        a->available == b->available;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadComponentCatalogueBridgeTransferTails(UmiRadComponentCatalogueBridge *value)
{
    (void)value;
    {
        size_t used = strlen(value->design_component_id) + 1U;
        memset(value->design_component_id + used, 0xa5, sizeof(value->design_component_id) - used);
    }
    {
        size_t used = strlen(value->designer_type) + 1U;
        memset(value->designer_type + used, 0xa5, sizeof(value->designer_type) - used);
    }
    {
        size_t used = strlen(value->family) + 1U;
        memset(value->family + used, 0xa5, sizeof(value->family) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadComponentCatalogueBridgeTransferMalformed(const UmiRadComponentCatalogueBridge *sample)
{
    (void)sample;
    {
        UmiRadComponentCatalogueBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.design_component_id, 'x', sizeof(invalid.design_component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_component_catalogue_bridge_is_valid(&invalid)) ||
            umi_rad_component_catalogue_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated design_component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadComponentCatalogueBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.designer_type, 'x', sizeof(invalid.designer_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_component_catalogue_bridge_is_valid(&invalid)) ||
            umi_rad_component_catalogue_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated designer_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadComponentCatalogueBridge invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.family, 'x', sizeof(invalid.family));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_component_catalogue_bridge_is_valid(&invalid)) ||
            umi_rad_component_catalogue_bridge_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated family was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadComponentCatalogueBridgeTransferCases, UmiRadComponentCatalogueBridge,
    umi_rad_component_catalogue_bridge_archive_encode, umi_rad_component_catalogue_bridge_archive_decode,
    UmiRadComponentCatalogueBridgeTransferEqual, UmiRadComponentCatalogueBridgeTransferTails, UmiRadComponentCatalogueBridgeTransferMalformed)

int main(void){UmiRadComponentCatalogueBridge item;CHECK(umi_rad_component_catalogue_bridge_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_component_catalogue_bridge_is_valid(&item));
    if (UmiRadComponentCatalogueBridgeTransferCases(&item) != 0) return 1;
return 0;}
