/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_intelligence_workbench/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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
#include "umicom/editor/intelligence_workbench/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/editor/intelligence_workbench/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiEditorIntelEntryTransferEqual(const UmiEditorIntelEntry *a, const UmiEditorIntelEntry *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->detail, b->detail) == 0 &&
        strcmp(a->location.path, b->location.path) == 0 &&
        a->location.range.start.line == b->location.range.start.line &&
        a->location.range.start.column == b->location.range.start.column &&
        a->location.range.end.line == b->location.range.end.line &&
        a->location.range.end.column == b->location.range.end.column &&
        a->score == b->score &&
        a->flags == b->flags &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiEditorIntelEntryTransferTails(UmiEditorIntelEntry *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
    {
        size_t used = strlen(value->location.path) + 1U;
        memset(value->location.path + used, 0xa5, sizeof(value->location.path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiEditorIntelEntryTransferMalformed(const UmiEditorIntelEntry *sample)
{
    (void)sample;
    {
        UmiEditorIntelEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_entry_valid(&invalid)) ||
            umi_editor_intel_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorIntelEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_entry_valid(&invalid)) ||
            umi_editor_intel_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorIntelEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.detail, 'x', sizeof(invalid.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_entry_valid(&invalid)) ||
            umi_editor_intel_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated detail was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiEditorIntelEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.location.path, 'x', sizeof(invalid.location.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_editor_intel_entry_valid(&invalid)) ||
            umi_editor_intel_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated location.path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiEditorIntelEntryTransferCases, UmiEditorIntelEntry,
    umi_editor_intel_entry_archive_encode, umi_editor_intel_entry_archive_decode,
    UmiEditorIntelEntryTransferEqual, UmiEditorIntelEntryTransferTails, UmiEditorIntelEntryTransferMalformed)

int main(void){UmiEditorIntelEntry entry;UmiEditorIntelRange range={{5U,9U},{2U,1U}};/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_intel_entry_init(&entry,"entry-1","main","src/main.c",range)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_intel_entry_valid(&entry))return 2;
    if (UmiEditorIntelEntryTransferCases(&entry) != 0) return 1;
/* Preserve the original failure result so the caller can respond to the correct cause. */ if(entry.location.range.start.line!=2U||entry.location.range.end.line!=5U)return 3;return 0;}
