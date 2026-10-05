/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_tree_node.c
 *
 * PURPOSE:
 *   Exercise the tree node enterprise UI capability.
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
#include "umicom/ui/enterprise/tree_node.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/tree_node.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntTreeNodeTransferEqual(const UmiUiEntTreeNode *a, const UmiUiEntTreeNode *b)
{
    return strcmp(a->node_id, b->node_id) == 0 &&
        strcmp(a->parent_id, b->parent_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->depth == b->depth &&
        a->has_children == b->has_children &&
        a->children_loaded == b->children_loaded &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntTreeNodeTransferTails(UmiUiEntTreeNode *value)
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
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntTreeNodeTransferMalformed(const UmiUiEntTreeNode *sample)
{
    (void)sample;
    {
        UmiUiEntTreeNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.node_id, 'x', sizeof(invalid.node_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_tree_node_validate(&invalid)) ||
            umi_ui_ent_tree_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated node_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntTreeNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id, 'x', sizeof(invalid.parent_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_tree_node_validate(&invalid)) ||
            umi_ui_ent_tree_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntTreeNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_tree_node_validate(&invalid)) ||
            umi_ui_ent_tree_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntTreeNodeTransferCases, UmiUiEntTreeNode,
    umi_ui_ent_tree_node_archive_encode, umi_ui_ent_tree_node_archive_decode,
    UmiUiEntTreeNodeTransferEqual, UmiUiEntTreeNodeTransferTails, UmiUiEntTreeNodeTransferMalformed)

int main(void){UmiUiEntTreeNode v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_tree_node_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.node_id,sizeof v.node_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_tree_node_validate(&v))return 9;
    if (UmiUiEntTreeNodeTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
