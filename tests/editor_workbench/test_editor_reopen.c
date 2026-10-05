/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_reopen.c
 *
 * PURPOSE:
 *   Implement the test editor reopen behavior for
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
#include "umicom/editor/workbench/editor_reopen.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/editor_reopen.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbEditorReopenTransferEqual(const UmiEditorWbEditorReopen *a, const UmiEditorWbEditorReopen *b)
{
    return strcmp(a->resource, b->resource) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        a->mode == b->mode &&
        a->force == b->force;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbEditorReopenTransferTails(UmiEditorWbEditorReopen *value)
{
    (void)value;
    {
        size_t used = strlen(value->resource) + 1U;
        memset(value->resource + used, 0xa5, sizeof(value->resource) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbEditorReopenTransferMalformed(const UmiEditorWbEditorReopen *sample)
{
    (void)sample;
    {
        UmiEditorWbEditorReopen invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resource, 'x', sizeof(invalid.resource));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_reopen_valid(&invalid)) ||
            umi_editor_wb_editor_reopen_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resource was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorWbEditorReopen invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_reopen_valid(&invalid)) ||
            umi_editor_wb_editor_reopen_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbEditorReopenTransferCases, UmiEditorWbEditorReopen,
    umi_editor_wb_editor_reopen_archive_encode, umi_editor_wb_editor_reopen_archive_decode,
    UmiEditorWbEditorReopenTransferEqual, UmiEditorWbEditorReopenTransferTails, UmiEditorWbEditorReopenTransferMalformed)

int main(void){ UmiEditorWbEditorReopen s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_reopen_init(&s,"a.c","main")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_reopen_valid(&s))return 2;
    if (UmiEditorWbEditorReopenTransferCases(&s) != 0) return 1;
 return 0; }
