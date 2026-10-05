/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_workspace_profile.c
 *
 * PURPOSE:
 *   Implement the test workspace profile behavior for
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
#include "umicom/editor/workbench/workspace_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/workspace_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbWorkspaceProfileTransferEqual(const UmiEditorWbWorkspaceProfile *a, const UmiEditorWbWorkspaceProfile *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->text, b->text) == 0 &&
        a->primary == b->primary &&
        a->secondary == b->secondary &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbWorkspaceProfileTransferTails(UmiEditorWbWorkspaceProfile *value)
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
static int UmiEditorWbWorkspaceProfileTransferMalformed(const UmiEditorWbWorkspaceProfile *sample)
{
    (void)sample;
    {
        UmiEditorWbWorkspaceProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_workspace_profile_valid(&invalid)) ||
            umi_editor_wb_workspace_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorWbWorkspaceProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_workspace_profile_valid(&invalid)) ||
            umi_editor_wb_workspace_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbWorkspaceProfileTransferCases, UmiEditorWbWorkspaceProfile,
    umi_editor_wb_workspace_profile_archive_encode, umi_editor_wb_workspace_profile_archive_decode,
    UmiEditorWbWorkspaceProfileTransferEqual, UmiEditorWbWorkspaceProfileTransferTails, UmiEditorWbWorkspaceProfileTransferMalformed)

int main(void){ UmiEditorWbWorkspaceProfile s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_workspace_profile_init(&s,"id","text")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_workspace_profile_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_workspace_profile_valid(&s)||s.primary!=3U)return 3;
    if (UmiEditorWbWorkspaceProfileTransferCases(&s) != 0) return 1;
 return 0; }
