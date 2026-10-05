/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workbench/test_workbench_types.c
 *
 * PURPOSE:
 *   Implement the test workbench types behavior for
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
#include "umicom/test/workbench/workbench_types.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/test/workbench/workbench_types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTestWorkbenchEntryTransferEqual(const UmiTestWorkbenchEntry *a, const UmiTestWorkbenchEntry *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->detail, b->detail) == 0 &&
        a->state == b->state &&
        a->flags == b->flags &&
        a->score == b->score &&
        a->duration_us == b->duration_us &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTestWorkbenchEntryTransferTails(UmiTestWorkbenchEntry *value)
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
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTestWorkbenchEntryTransferMalformed(const UmiTestWorkbenchEntry *sample)
{
    (void)sample;
    {
        UmiTestWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_test_workbench_entry_valid(&invalid)) ||
            umi_test_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_test_workbench_entry_valid(&invalid)) ||
            umi_test_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTestWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.detail, 'x', sizeof(invalid.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_test_workbench_entry_valid(&invalid)) ||
            umi_test_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated detail was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTestWorkbenchEntryTransferCases, UmiTestWorkbenchEntry,
    umi_test_workbench_entry_archive_encode, umi_test_workbench_entry_archive_decode,
    UmiTestWorkbenchEntryTransferEqual, UmiTestWorkbenchEntryTransferTails, UmiTestWorkbenchEntryTransferMalformed)

int main(void){UmiTestWorkbenchEntry e;assert(umi_test_workbench_entry_init(&e,"root","All Tests")==UMI_STATUS_OK);assert(umi_test_workbench_entry_set_detail(&e,"workspace")==UMI_STATUS_OK);assert(umi_test_workbench_entry_set_state(&e,UMI_TEST_WORKBENCH_STATE_READY)==UMI_STATUS_OK);assert(umi_test_workbench_entry_valid(&e));
    if (UmiTestWorkbenchEntryTransferCases(&e) != 0) return 1;
assert(e.revision==3U);return 0;}
