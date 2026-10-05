/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_dirty_document_state.c
 *
 * PURPOSE:
 *   Implement the test dirty document state behavior for
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
#include "umicom/editor/workbench/dirty_document_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/dirty_document_state.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbDirtyDocumentStateTransferEqual(const UmiEditorWbDirtyDocumentState *a, const UmiEditorWbDirtyDocumentState *b)
{
    return strcmp(a->item_id, b->item_id) == 0 &&
        a->enabled == b->enabled &&
        a->promoted == b->promoted &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbDirtyDocumentStateTransferTails(UmiEditorWbDirtyDocumentState *value)
{
    (void)value;
    {
        size_t used = strlen(value->item_id) + 1U;
        memset(value->item_id + used, 0xa5, sizeof(value->item_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbDirtyDocumentStateTransferMalformed(const UmiEditorWbDirtyDocumentState *sample)
{
    (void)sample;
    {
        UmiEditorWbDirtyDocumentState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.item_id, 'x', sizeof(invalid.item_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_dirty_document_state_valid(&invalid)) ||
            umi_editor_wb_dirty_document_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated item_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbDirtyDocumentStateTransferCases, UmiEditorWbDirtyDocumentState,
    umi_editor_wb_dirty_document_state_archive_encode, umi_editor_wb_dirty_document_state_archive_decode,
    UmiEditorWbDirtyDocumentStateTransferEqual, UmiEditorWbDirtyDocumentStateTransferTails, UmiEditorWbDirtyDocumentStateTransferMalformed)

int main(void){ UmiEditorWbDirtyDocumentState s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_dirty_document_state_init(&s,"item",false)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_dirty_document_state_set(&s,true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_dirty_document_state_valid(&s)||!s.enabled)return 3;
    if (UmiEditorWbDirtyDocumentStateTransferCases(&s) != 0) return 1;
 return 0; }
