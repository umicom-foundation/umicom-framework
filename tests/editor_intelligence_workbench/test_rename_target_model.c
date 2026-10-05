/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_rename_target_model.c
 *
 * PURPOSE:
 *   Implement the test rename target model behavior for
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
#include "umicom/editor/intelligence_workbench/rename_target_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/intelligence_workbench/rename_target_model.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorIntelRenameTargetModelTransferEqual(const UmiEditorIntelRenameTargetModel *a, const UmiEditorIntelRenameTargetModel *b)
{
    return strcmp(a->value.id, b->value.id) == 0 &&
        strcmp(a->value.label, b->value.label) == 0 &&
        strcmp(a->value.detail, b->value.detail) == 0 &&
        strcmp(a->value.location.path, b->value.location.path) == 0 &&
        a->value.location.range.start.line == b->value.location.range.start.line &&
        a->value.location.range.start.column == b->value.location.range.start.column &&
        a->value.location.range.end.line == b->value.location.range.end.line &&
        a->value.location.range.end.column == b->value.location.range.end.column &&
        a->value.score == b->value.score &&
        a->value.flags == b->value.flags &&
        a->value.revision == b->value.revision &&
        a->applicability == b->applicability &&
        a->selected == b->selected &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorIntelRenameTargetModelTransferTails(UmiEditorIntelRenameTargetModel *value)
{
    (void)value;
    {
        size_t used = strlen(value->value.id) + 1U;
        memset(value->value.id + used, 0xa5, sizeof(value->value.id) - used);
    }
    {
        size_t used = strlen(value->value.label) + 1U;
        memset(value->value.label + used, 0xa5, sizeof(value->value.label) - used);
    }
    {
        size_t used = strlen(value->value.detail) + 1U;
        memset(value->value.detail + used, 0xa5, sizeof(value->value.detail) - used);
    }
    {
        size_t used = strlen(value->value.location.path) + 1U;
        memset(value->value.location.path + used, 0xa5, sizeof(value->value.location.path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorIntelRenameTargetModelTransferMalformed(const UmiEditorIntelRenameTargetModel *sample)
{
    (void)sample;
    {
        UmiEditorIntelRenameTargetModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.id, 'x', sizeof(invalid.value.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_rename_target_model_valid(&invalid)) ||
            umi_editor_intel_rename_target_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorIntelRenameTargetModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.label, 'x', sizeof(invalid.value.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_rename_target_model_valid(&invalid)) ||
            umi_editor_intel_rename_target_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorIntelRenameTargetModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.detail, 'x', sizeof(invalid.value.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_rename_target_model_valid(&invalid)) ||
            umi_editor_intel_rename_target_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.detail was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorIntelRenameTargetModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value.location.path, 'x', sizeof(invalid.value.location.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_rename_target_model_valid(&invalid)) ||
            umi_editor_intel_rename_target_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value.location.path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorIntelRenameTargetModelTransferCases, UmiEditorIntelRenameTargetModel,
    umi_editor_intel_rename_target_model_archive_encode, umi_editor_intel_rename_target_model_archive_decode,
    UmiEditorIntelRenameTargetModelTransferEqual, UmiEditorIntelRenameTargetModelTransferTails, UmiEditorIntelRenameTargetModelTransferMalformed)

int main(void){UmiEditorIntelRenameTargetModel model;UmiEditorIntelRange range={{3U,4U},{3U,12U}};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_rename_target_model_init(&model,"model-1","item","src/main.c",range)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_rename_target_model_set_score(&model,91U)!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_rename_target_model_set_selected(&model,true)!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_rename_target_model_valid(&model)||model.value.score!=91U||!model.selected)return 4;
    if (UmiEditorIntelRenameTargetModelTransferCases(&model) != 0) return 1;
return 0;}
