/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_envelope_record.c
 *
 * PURPOSE:
 *   Verify envelope record validation, mutation and stable hashing.
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

#include "umicom/workbench_context_event/envelope_record.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_event/envelope_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextEventEnvelopeRecordTransferEqual(const UmiWorkbenchContextEventEnvelopeRecord *a, const UmiWorkbenchContextEventEnvelopeRecord *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->record_id, b->record_id) == 0 &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->event_kind == b->event_kind &&
        a->context_kind == b->context_kind &&
        a->priority == b->priority &&
        a->state == b->state &&
        a->flags == b->flags &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextEventEnvelopeRecordTransferTails(UmiWorkbenchContextEventEnvelopeRecord *value)
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
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextEventEnvelopeRecordTransferMalformed(const UmiWorkbenchContextEventEnvelopeRecord *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextEventEnvelopeRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.record_id, 'x', sizeof(invalid.record_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_event_envelope_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_event_envelope_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated record_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextEventEnvelopeRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_event_envelope_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_event_envelope_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextEventEnvelopeRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_event_envelope_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_event_envelope_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextEventEnvelopeRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_event_envelope_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_event_envelope_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextEventEnvelopeRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_event_envelope_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_event_envelope_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextEventEnvelopeRecordTransferCases, UmiWorkbenchContextEventEnvelopeRecord,
    umi_workbench_context_event_envelope_record_archive_encode, umi_workbench_context_event_envelope_record_archive_decode,
    UmiWorkbenchContextEventEnvelopeRecordTransferEqual, UmiWorkbenchContextEventEnvelopeRecordTransferTails, UmiWorkbenchContextEventEnvelopeRecordTransferMalformed)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiWorkbenchContextEventEnvelopeRecordConditionalArchiveCases, UmiWorkbenchContextEventEnvelopeRecord,
    umi_workbench_context_event_envelope_record_archive_encode, umi_workbench_context_event_envelope_record_archive_decode_if_unchanged,
    UmiWorkbenchContextEventEnvelopeRecordTransferEqual, UmiWorkbenchContextEventEnvelopeRecordTransferTails, UmiWorkbenchContextEventEnvelopeRecordTransferMalformed)


/* Exercise the accepted domain fixture through reviewed publication and
 * legacy mutation boundaries. These checks stay active in release builds. */
#include "../record_integrity/reviewed_value_cases.h"
UMI_TEST_REVIEWED_VALUE(UmiWorkbenchContextEventEnvelopeRecordReviewedCases, UmiWorkbenchContextEventEnvelopeRecord,
    record_id, label, umi_workbench_context_event_envelope_record_replace_if_current, UmiWorkbenchContextEventEnvelopeRecordTransferEqual)
static int UmiWorkbenchContextEventEnvelopeRecordMutationGuards(const UmiWorkbenchContextEventEnvelopeRecord *sample)
{
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextEventEnvelopeRecord, source_id, umi_workbench_context_event_envelope_record_set_source, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextEventEnvelopeRecord, subject_id, umi_workbench_context_event_envelope_record_set_subject, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextEventEnvelopeRecord, group_id, umi_workbench_context_event_envelope_record_set_group, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextEventEnvelopeRecord, label, umi_workbench_context_event_envelope_record_set_label, sample);
    UmiWorkbenchContextEventEnvelopeRecord value = *sample;
    value.revision = UINT64_MAX - 1U;
    umi_workbench_context_event_envelope_record_touch(&value, 33U, 44U);
    if (value.revision != UINT64_MAX || value.sequence != 33U || value.timestamp_ms != 44U) return 1;
    unsigned char before[sizeof value]; memcpy(before, &value, sizeof value);
    umi_workbench_context_event_envelope_record_touch(&value, 55U, 66U);
    if (memcmp(before, &value, sizeof value) != 0) return 1;
    return 0;
}

int main(void)
{
    UmiWorkbenchContextEventEnvelopeRecord record;
    uint64_t first_hash;

    umi_workbench_context_event_envelope_record_init(
        &record,
        "envelope_record-record");
    assert(umi_workbench_context_event_envelope_record_validate(
        &record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextEventEnvelopeRecordTransferCases(&record) != 0) return 1;
    if (UmiWorkbenchContextEventEnvelopeRecordConditionalArchiveCases(&record) != 0) return 1;
    if (UmiWorkbenchContextEventEnvelopeRecordReviewedCases(&record) != 0) return 1;
    if (UmiWorkbenchContextEventEnvelopeRecordMutationGuards(&record) != 0) return 1;


    assert(umi_workbench_context_event_envelope_record_set_source(
        &record,
        "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_envelope_record_set_subject(
        &record,
        "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_envelope_record_set_group(
        &record,
        "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_envelope_record_set_label(
        &record,
        "label") == UMI_STATUS_OK);

    record.event_kind =
        UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.priority =
        UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    record.state =
        UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;

    first_hash = umi_workbench_context_event_envelope_record_hash(
        &record);
    assert(first_hash != 0U);
    umi_workbench_context_event_envelope_record_touch(
        &record,
        7U,
        1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.subject_id, "subject") == 0);
    return 0;
}
