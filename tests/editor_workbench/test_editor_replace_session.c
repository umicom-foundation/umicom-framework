/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_replace_session.c
 *
 * PURPOSE:
 *   Implement the test editor replace session behavior for
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
#include "umicom/editor/workbench/editor_replace_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/editor_replace_session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbEditorReplaceSessionTransferEqual(const UmiEditorWbEditorReplaceSession *a, const UmiEditorWbEditorReplaceSession *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->text, b->text) == 0 &&
        a->primary == b->primary &&
        a->secondary == b->secondary &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbEditorReplaceSessionTransferTails(UmiEditorWbEditorReplaceSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbEditorReplaceSessionTransferMalformed(const UmiEditorWbEditorReplaceSession *sample)
{
    (void)sample;
    {
        UmiEditorWbEditorReplaceSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_replace_session_valid(&invalid)) ||
            umi_editor_wb_editor_replace_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorWbEditorReplaceSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_replace_session_valid(&invalid)) ||
            umi_editor_wb_editor_replace_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbEditorReplaceSessionTransferCases, UmiEditorWbEditorReplaceSession,
    umi_editor_wb_editor_replace_session_archive_encode, umi_editor_wb_editor_replace_session_archive_decode,
    UmiEditorWbEditorReplaceSessionTransferEqual, UmiEditorWbEditorReplaceSessionTransferTails, UmiEditorWbEditorReplaceSessionTransferMalformed)

int main(void){ UmiEditorWbEditorReplaceSession s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_replace_session_init(&s,"id","text")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_replace_session_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_replace_session_valid(&s)||s.primary!=3U)return 3;
    if (UmiEditorWbEditorReplaceSessionTransferCases(&s) != 0) return 1;
 return 0; }
