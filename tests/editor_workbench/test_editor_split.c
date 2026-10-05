/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_split.c
 *
 * PURPOSE:
 *   Implement the test editor split behavior for
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
#include "umicom/editor/workbench/editor_split.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/editor_split.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbEditorSplitTransferEqual(const UmiEditorWbEditorSplit *a, const UmiEditorWbEditorSplit *b)
{
    return strcmp(a->split_id, b->split_id) == 0 &&
        a->orientation == b->orientation &&
        a->ratio == b->ratio;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbEditorSplitTransferTails(UmiEditorWbEditorSplit *value)
{
    (void)value;
    {
        size_t used = strlen(value->split_id) + 1U;
        memset(value->split_id + used, 0xa5, sizeof(value->split_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbEditorSplitTransferMalformed(const UmiEditorWbEditorSplit *sample)
{
    (void)sample;
    {
        UmiEditorWbEditorSplit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.split_id, 'x', sizeof(invalid.split_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_split_valid(&invalid)) ||
            umi_editor_wb_editor_split_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated split_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbEditorSplitTransferCases, UmiEditorWbEditorSplit,
    umi_editor_wb_editor_split_archive_encode, umi_editor_wb_editor_split_archive_decode,
    UmiEditorWbEditorSplitTransferEqual, UmiEditorWbEditorSplitTransferTails, UmiEditorWbEditorSplitTransferMalformed)

int main(void){ UmiEditorWbEditorSplit s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_split_init(&s,"s",UMI_EDITOR_WB_HORIZONTAL,0.5)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_split_set_ratio(&s,0.95)!=UMI_STATUS_INVALID_ARGUMENT)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_split_valid(&s))return 3;
    if (UmiEditorWbEditorSplitTransferCases(&s) != 0) return 1;
 return 0; }
