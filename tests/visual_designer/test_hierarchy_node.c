/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_hierarchy_node.c
 *
 * PURPOSE:
 *   Validate represent one node in the designer object hierarchy.
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
#include "umicom/designer/visual_designer/hierarchy_node.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/hierarchy_node.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadHierarchyNodeTransferEqual(const UmiRadHierarchyNode *a, const UmiRadHierarchyNode *b)
{
    return strcmp(a->node_id, b->node_id) == 0 &&
        strcmp(a->parent_id, b->parent_id) == 0 &&
        a->order == b->order &&
        a->expanded == b->expanded;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadHierarchyNodeTransferTails(UmiRadHierarchyNode *value)
{
    (void)value;
    {
        size_t used = strlen(value->node_id) + 1U;
        memset(value->node_id + used, 0xa5, sizeof(value->node_id) - used);
    }
    {
        size_t used = strlen(value->parent_id) + 1U;
        memset(value->parent_id + used, 0xa5, sizeof(value->parent_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadHierarchyNodeTransferMalformed(const UmiRadHierarchyNode *sample)
{
    (void)sample;
    {
        UmiRadHierarchyNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.node_id, 'x', sizeof(invalid.node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_hierarchy_node_is_valid(&invalid)) ||
            umi_rad_hierarchy_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated node_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadHierarchyNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id, 'x', sizeof(invalid.parent_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_hierarchy_node_is_valid(&invalid)) ||
            umi_rad_hierarchy_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadHierarchyNodeTransferCases, UmiRadHierarchyNode,
    umi_rad_hierarchy_node_archive_encode, umi_rad_hierarchy_node_archive_decode,
    UmiRadHierarchyNodeTransferEqual, UmiRadHierarchyNodeTransferTails, UmiRadHierarchyNodeTransferMalformed)

int main(void){UmiRadHierarchyNode item;CHECK(umi_rad_hierarchy_node_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_hierarchy_node_is_valid(&item));
    if (UmiRadHierarchyNodeTransferCases(&item) != 0) return 1;
return 0;}
