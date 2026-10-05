/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_inspector_editor.c
 *
 * PURPOSE:
 *   Exercise the inspector editor enterprise UI capability.
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
#include "umicom/ui/enterprise/inspector_editor.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/inspector_editor.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntInspectorEditorTransferEqual(const UmiUiEntInspectorEditor *a, const UmiUiEntInspectorEditor *b)
{
    return strcmp(a->property_id, b->property_id) == 0 &&
        strcmp(a->editor_kind, b->editor_kind) == 0 &&
        a->choice_count == b->choice_count &&
        a->multiline == b->multiline &&
        a->read_only == b->read_only;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntInspectorEditorTransferTails(UmiUiEntInspectorEditor *value)
{
    (void)value;
    {
        size_t used = strlen(value->property_id) + 1U;
        memset(value->property_id + used, 0xa5, sizeof(value->property_id) - used);
    }
    {
        size_t used = strlen(value->editor_kind) + 1U;
        memset(value->editor_kind + used, 0xa5, sizeof(value->editor_kind) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntInspectorEditorTransferMalformed(const UmiUiEntInspectorEditor *sample)
{
    (void)sample;
    {
        UmiUiEntInspectorEditor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_id, 'x', sizeof(invalid.property_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_editor_validate(&invalid)) ||
            umi_ui_ent_inspector_editor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiEntInspectorEditor invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.editor_kind, 'x', sizeof(invalid.editor_kind));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_ui_ent_inspector_editor_validate(&invalid)) ||
            umi_ui_ent_inspector_editor_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated editor_kind was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntInspectorEditorTransferCases, UmiUiEntInspectorEditor,
    umi_ui_ent_inspector_editor_archive_encode, umi_ui_ent_inspector_editor_archive_decode,
    UmiUiEntInspectorEditorTransferEqual, UmiUiEntInspectorEditorTransferTails, UmiUiEntInspectorEditorTransferMalformed)

int main(void){UmiUiEntInspectorEditor v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_inspector_editor_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.property_id,sizeof v.property_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.editor_kind,sizeof v.editor_kind,"text")!=UMI_STATUS_OK)return 3;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_inspector_editor_validate(&v))return 9;
    if (UmiUiEntInspectorEditorTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
