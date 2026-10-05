/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_location.c
 *
 * PURPOSE:
 *   Implement the test editor location behavior for
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
#include "umicom/editor/workbench/editor_location.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/workbench/editor_location.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorWbEditorLocationTransferEqual(const UmiEditorWbEditorLocation *a, const UmiEditorWbEditorLocation *b)
{
    return strcmp(a->path, b->path) == 0 &&
        a->position.line == b->position.line &&
        a->position.column == b->position.column;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorWbEditorLocationTransferTails(UmiEditorWbEditorLocation *value)
{
    (void)value;
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorWbEditorLocationTransferMalformed(const UmiEditorWbEditorLocation *sample)
{
    (void)sample;
    {
        UmiEditorWbEditorLocation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_wb_editor_location_valid(&invalid)) ||
            umi_editor_wb_editor_location_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorWbEditorLocationTransferCases, UmiEditorWbEditorLocation,
    umi_editor_wb_editor_location_archive_encode, umi_editor_wb_editor_location_archive_decode,
    UmiEditorWbEditorLocationTransferEqual, UmiEditorWbEditorLocationTransferTails, UmiEditorWbEditorLocationTransferMalformed)

int main(void){ UmiEditorWbEditorLocation l; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_location_init(&l,"a.c",3U,4U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_location_valid(&l))return 2;
    if (UmiEditorWbEditorLocationTransferCases(&l) != 0) return 1;
 /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_location_init(&l,"a.c",0U,1U)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
