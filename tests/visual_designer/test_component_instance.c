/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_component_instance.c
 *
 * PURPOSE:
 *   Validate represent one semantic component instance on a designer document.
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
#include "umicom/designer/visual_designer/component_instance.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/component_instance.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadComponentInstanceTransferEqual(const UmiRadComponentInstance *a, const UmiRadComponentInstance *b)
{
    return strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->component_type, b->component_type) == 0 &&
        strcmp(a->parent_id, b->parent_id) == 0 &&
        a->bounds.x == b->bounds.x &&
        a->bounds.y == b->bounds.y &&
        a->bounds.width == b->bounds.width &&
        a->bounds.height == b->bounds.height &&
        a->visible == b->visible;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadComponentInstanceTransferTails(UmiRadComponentInstance *value)
{
    (void)value;
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->component_type) + 1U;
        memset(value->component_type + used, 0xa5, sizeof(value->component_type) - used);
    }
    {
        size_t used = strlen(value->parent_id) + 1U;
        memset(value->parent_id + used, 0xa5, sizeof(value->parent_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadComponentInstanceTransferMalformed(const UmiRadComponentInstance *sample)
{
    (void)sample;
    {
        UmiRadComponentInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_component_instance_is_valid(&invalid)) ||
            umi_rad_component_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadComponentInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_type, 'x', sizeof(invalid.component_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_component_instance_is_valid(&invalid)) ||
            umi_rad_component_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadComponentInstance invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id, 'x', sizeof(invalid.parent_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_component_instance_is_valid(&invalid)) ||
            umi_rad_component_instance_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadComponentInstanceTransferCases, UmiRadComponentInstance,
    umi_rad_component_instance_archive_encode, umi_rad_component_instance_archive_decode,
    UmiRadComponentInstanceTransferEqual, UmiRadComponentInstanceTransferTails, UmiRadComponentInstanceTransferMalformed)

int main(void){UmiRadComponentInstance item;CHECK(umi_rad_component_instance_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_component_instance_is_valid(&item));
    if (UmiRadComponentInstanceTransferCases(&item) != 0) return 1;
return 0;}
