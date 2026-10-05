/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_layout.c
 *
 * PURPOSE:
 *   Implement the test editor layout behavior for
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
#include "umicom/editor/workbench/editor_layout.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/editor_layout.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbEditorLayoutTransferEqual(const UmiEditorWbEditorLayout *a, const UmiEditorWbEditorLayout *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->parent_id, b->parent_id) == 0 &&
        a->item_count == b->item_count &&
        a->active_index == b->active_index &&
        a->active == b->active &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbEditorLayoutTransferTails(UmiEditorWbEditorLayout *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->parent_id) + 1U;
        memset(value->parent_id + used, 0xa5, sizeof(value->parent_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbEditorLayoutTransferMalformed(const UmiEditorWbEditorLayout *sample)
{
    (void)sample;
    {
        UmiEditorWbEditorLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_layout_valid(&invalid)) ||
            umi_editor_wb_editor_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorWbEditorLayout invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id, 'x', sizeof(invalid.parent_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_layout_valid(&invalid)) ||
            umi_editor_wb_editor_layout_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbEditorLayoutTransferCases, UmiEditorWbEditorLayout,
    umi_editor_wb_editor_layout_archive_encode, umi_editor_wb_editor_layout_archive_decode,
    UmiEditorWbEditorLayoutTransferEqual, UmiEditorWbEditorLayoutTransferTails, UmiEditorWbEditorLayoutTransferMalformed)

int main(void){ UmiEditorWbEditorLayout s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_layout_init(&s,"id","")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_layout_set_count(&s,3U,1U)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_layout_valid(&s)||s.active_index!=1U)return 3;
    if (UmiEditorWbEditorLayoutTransferCases(&s) != 0) return 1;
 return 0; }
