/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_group_node.c
 *
 * PURPOSE:
 *   Exercise the group node enterprise UI capability.
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
#include "umicom/ui/enterprise/group_node.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/group_node.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntGroupNodeTransferEqual(const UmiUiEntGroupNode *a, const UmiUiEntGroupNode *b)
{
    return strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->first_row == b->first_row &&
        a->row_count == b->row_count &&
        a->depth == b->depth &&
        a->expanded == b->expanded;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntGroupNodeTransferTails(UmiUiEntGroupNode *value)
{
    (void)value;
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntGroupNodeTransferMalformed(const UmiUiEntGroupNode *sample)
{
    (void)sample;
    {
        UmiUiEntGroupNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_group_node_validate(&invalid)) ||
            umi_ui_ent_group_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntGroupNode invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_group_node_validate(&invalid)) ||
            umi_ui_ent_group_node_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntGroupNodeTransferCases, UmiUiEntGroupNode,
    umi_ui_ent_group_node_archive_encode, umi_ui_ent_group_node_archive_decode,
    UmiUiEntGroupNodeTransferEqual, UmiUiEntGroupNodeTransferTails, UmiUiEntGroupNodeTransferMalformed)

int main(void){UmiUiEntGroupNode v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_group_node_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.group_id,sizeof v.group_id,"id")!=UMI_STATUS_OK)return 2;v.row_count=1U;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_group_node_validate(&v))return 9;
    if (UmiUiEntGroupNodeTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
