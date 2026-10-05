/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_item.c
 *
 * PURPOSE:
 *   Implement the test editor item behavior for
 *   Umicom Framework.
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
#include "umicom/editor/workbench/editor_item.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/editor_item.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbEditorItemTransferEqual(const UmiEditorWbEditorItem *a, const UmiEditorWbEditorItem *b)
{
    return strcmp(a->item_id, b->item_id) == 0 &&
        strcmp(a->path, b->path) == 0 &&
        a->open_mode == b->open_mode &&
        a->dirty == b->dirty &&
        a->pinned == b->pinned &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbEditorItemTransferTails(UmiEditorWbEditorItem *value)
{
    (void)value;
    {
        size_t used = strlen(value->item_id) + 1U;
        memset(value->item_id + used, 0xa5, sizeof(value->item_id) - used);
    }
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbEditorItemTransferMalformed(const UmiEditorWbEditorItem *sample)
{
    (void)sample;
    {
        UmiEditorWbEditorItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.item_id, 'x', sizeof(invalid.item_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_item_valid(&invalid)) ||
            umi_editor_wb_editor_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated item_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorWbEditorItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_item_valid(&invalid)) ||
            umi_editor_wb_editor_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbEditorItemTransferCases, UmiEditorWbEditorItem,
    umi_editor_wb_editor_item_archive_encode, umi_editor_wb_editor_item_archive_decode,
    UmiEditorWbEditorItemTransferEqual, UmiEditorWbEditorItemTransferTails, UmiEditorWbEditorItemTransferMalformed)

int main(void){ UmiEditorWbEditorItem x; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_item_init(&x,"e1","a.c",UMI_EDITOR_WB_OPEN_PREVIEW)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_item_valid(&x))return 2;
    if (UmiEditorWbEditorItemTransferCases(&x) != 0) return 1;
 /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_item_set_dirty(&x,true)!=UMI_STATUS_OK||!x.dirty)return 3; return 0; }
