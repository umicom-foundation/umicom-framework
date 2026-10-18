/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_focus_record.c
 *
 * PURPOSE:
 *   Verify the selection focus record contract and hash stability.
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
#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/focus_record.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_selection/focus_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchSelectionFocusRecordTransferEqual(const UmiWorkbenchSelectionFocusRecord *a, const UmiWorkbenchSelectionFocusRecord *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->record_id, b->record_id) == 0 &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        strcmp(a->related_id, b->related_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->description, b->description) == 0 &&
        a->selection_kind == b->selection_kind &&
        a->activation == b->activation &&
        a->state == b->state &&
        a->context_kind == b->context_kind &&
        a->flags == b->flags &&
        a->count == b->count &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchSelectionFocusRecordTransferTails(UmiWorkbenchSelectionFocusRecord *value)
{
    (void)value;
    {
        size_t used = strlen(value->record_id) + 1U;
        memset(value->record_id + used, 0xa5, sizeof(value->record_id) - used);
    }
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->subject_id) + 1U;
        memset(value->subject_id + used, 0xa5, sizeof(value->subject_id) - used);
    }
    {
        size_t used = strlen(value->related_id) + 1U;
        memset(value->related_id + used, 0xa5, sizeof(value->related_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->description) + 1U;
        memset(value->description + used, 0xa5, sizeof(value->description) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchSelectionFocusRecordTransferMalformed(const UmiWorkbenchSelectionFocusRecord *sample)
{
    (void)sample;
    {
        UmiWorkbenchSelectionFocusRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.record_id, 'x', sizeof(invalid.record_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_focus_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_focus_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated record_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionFocusRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_focus_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_focus_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionFocusRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_focus_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_focus_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionFocusRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.related_id, 'x', sizeof(invalid.related_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_focus_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_focus_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated related_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionFocusRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_focus_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_focus_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionFocusRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.description, 'x', sizeof(invalid.description));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_focus_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_focus_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated description was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchSelectionFocusRecordTransferCases, UmiWorkbenchSelectionFocusRecord,
    umi_workbench_selection_focus_record_archive_encode, umi_workbench_selection_focus_record_archive_decode,
    UmiWorkbenchSelectionFocusRecordTransferEqual, UmiWorkbenchSelectionFocusRecordTransferTails, UmiWorkbenchSelectionFocusRecordTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiWorkbenchSelectionFocusRecordTransferCasesWithWorkspace, UmiWorkbenchSelectionFocusRecord,
    umi_workbench_selection_focus_record_archive_encode, umi_workbench_selection_focus_record_archive_decode_with_workspace, UmiWorkbenchSelectionFocusRecordTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiWorkbenchSelectionFocusRecordConditionalArchiveCases, UmiWorkbenchSelectionFocusRecord,
    umi_workbench_selection_focus_record_archive_encode, umi_workbench_selection_focus_record_archive_decode_if_unchanged,
    UmiWorkbenchSelectionFocusRecordTransferEqual, UmiWorkbenchSelectionFocusRecordTransferTails, UmiWorkbenchSelectionFocusRecordTransferMalformed)


/* Exercise the accepted domain fixture through reviewed publication and
 * legacy mutation boundaries. These checks stay active in release builds. */
#include "../record_integrity/reviewed_value_cases.h"
UMI_TEST_REVIEWED_VALUE(UmiWorkbenchSelectionFocusRecordReviewedCases, UmiWorkbenchSelectionFocusRecord,
    record_id, description, umi_workbench_selection_focus_record_replace_if_current, UmiWorkbenchSelectionFocusRecordTransferEqual)
static int UmiWorkbenchSelectionFocusRecordMutationGuards(const UmiWorkbenchSelectionFocusRecord *sample)
{
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchSelectionFocusRecord, source_id, umi_workbench_selection_focus_record_set_source, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchSelectionFocusRecord, subject_id, umi_workbench_selection_focus_record_set_subject, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchSelectionFocusRecord, related_id, umi_workbench_selection_focus_record_set_related, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchSelectionFocusRecord, group_id, umi_workbench_selection_focus_record_set_group, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchSelectionFocusRecord, description, umi_workbench_selection_focus_record_set_description, sample);
    UmiWorkbenchSelectionFocusRecord value = *sample;
    value.revision = UINT64_MAX - 1U;
    umi_workbench_selection_focus_record_touch(&value, 33U, 44U);
    if (value.revision != UINT64_MAX || value.sequence != 33U || value.timestamp_ms != 44U) return 1;
    unsigned char before[sizeof value]; memcpy(before, &value, sizeof value);
    umi_workbench_selection_focus_record_touch(&value, 55U, 66U);
    if (memcmp(before, &value, sizeof value) != 0) return 1;
    return 0;
}

int main(void)
{
    UmiWorkbenchSelectionFocusRecord record;
    uint64_t hash;

    umi_workbench_selection_focus_record_init(
        &record, "focus_record-record");
    assert(umi_workbench_selection_focus_record_validate(
        &record) == UMI_STATUS_OK);
    if (UmiWorkbenchSelectionFocusRecordTransferCases(&record) != 0) return 1;
    if (UmiWorkbenchSelectionFocusRecordTransferCasesWithWorkspace(&record) != 0) return 1;
    if (UmiWorkbenchSelectionFocusRecordConditionalArchiveCases(&record) != 0) return 1;
    if (UmiWorkbenchSelectionFocusRecordReviewedCases(&record) != 0) return 1;
    if (UmiWorkbenchSelectionFocusRecordMutationGuards(&record) != 0) return 1;

    assert(umi_workbench_selection_focus_record_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_focus_record_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_focus_record_set_related(
        &record, "related") == UMI_STATUS_OK);
    assert(umi_workbench_selection_focus_record_set_group(
        &record, "green") == UMI_STATUS_OK);
    assert(umi_workbench_selection_focus_record_set_description(
        &record, "description") == UMI_STATUS_OK);
    record.selection_kind = UMI_WORKBENCH_SELECTION_TEST;
    record.context_kind = UMI_CONTEXT_KIND_SELECTION;
    record.state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    record.count = 3U;

    hash = umi_workbench_selection_focus_record_hash(&record);
    assert(hash != 0U);
    umi_workbench_selection_focus_record_touch(
        &record, 3U, 500U);
    assert(record.sequence == 3U);
    assert(record.timestamp_ms == 500U);
    assert(strcmp(record.related_id, "related") == 0);
    return 0;
}
