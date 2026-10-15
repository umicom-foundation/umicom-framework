/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_command.c
 *
 * PURPOSE:
 *   Verify the command contract, bounded mutation and stable hashing.
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

#include "umicom/workbench_context_source/command.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_source/command.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextSourceCommandTransferEqual(const UmiWorkbenchContextSourceCommand *a, const UmiWorkbenchContextSourceCommand *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->record_id, b->record_id) == 0 &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->source_kind == b->source_kind &&
        a->trigger == b->trigger &&
        a->state == b->state &&
        a->context_kind == b->context_kind &&
        a->flags == b->flags &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextSourceCommandTransferTails(UmiWorkbenchContextSourceCommand *value)
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
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
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
static int UmiWorkbenchContextSourceCommandTransferMalformed(const UmiWorkbenchContextSourceCommand *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextSourceCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.record_id, 'x', sizeof(invalid.record_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated record_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextSourceCommand invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_source_command_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_source_command_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextSourceCommandTransferCases, UmiWorkbenchContextSourceCommand,
    umi_workbench_context_source_command_archive_encode, umi_workbench_context_source_command_archive_decode,
    UmiWorkbenchContextSourceCommandTransferEqual, UmiWorkbenchContextSourceCommandTransferTails, UmiWorkbenchContextSourceCommandTransferMalformed)

/* Exercise the accepted domain fixture through reviewed publication and
 * legacy mutation boundaries. These checks stay active in release builds. */
#include "../record_integrity/reviewed_value_cases.h"
UMI_TEST_REVIEWED_VALUE(UmiWorkbenchContextSourceCommandReviewedCases, UmiWorkbenchContextSourceCommand,
    record_id, label, umi_workbench_context_source_command_replace_if_current, UmiWorkbenchContextSourceCommandTransferEqual)
static int UmiWorkbenchContextSourceCommandMutationGuards(const UmiWorkbenchContextSourceCommand *sample)
{
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextSourceCommand, source_id, umi_workbench_context_source_command_set_source, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextSourceCommand, panel_id, umi_workbench_context_source_command_set_panel, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextSourceCommand, subject_id, umi_workbench_context_source_command_set_subject, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextSourceCommand, group_id, umi_workbench_context_source_command_set_group, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextSourceCommand, label, umi_workbench_context_source_command_set_label, sample);
    UmiWorkbenchContextSourceCommand value = *sample;
    value.revision = UINT64_MAX - 1U;
    umi_workbench_context_source_command_touch(&value, 33U, 44U);
    if (value.revision != UINT64_MAX || value.sequence != 33U || value.timestamp_ms != 44U) return 1;
    unsigned char before[sizeof value]; memcpy(before, &value, sizeof value);
    umi_workbench_context_source_command_touch(&value, 55U, 66U);
    if (memcmp(before, &value, sizeof value) != 0) return 1;
    return 0;
}

int main(void)
{
    UmiWorkbenchContextSourceCommand record;
    uint64_t hash;

    umi_workbench_context_source_command_init(
        &record,
        "command-record");
    assert(umi_workbench_context_source_command_validate(
        &record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextSourceCommandTransferCases(&record) != 0) return 1;
    if (UmiWorkbenchContextSourceCommandReviewedCases(&record) != 0) return 1;
    if (UmiWorkbenchContextSourceCommandMutationGuards(&record) != 0) return 1;


    assert(umi_workbench_context_source_command_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_command_set_panel(
        &record, "panel") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_command_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_command_set_group(
        &record, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_source_command_set_label(
        &record, "label") == UMI_STATUS_OK);

    record.source_kind = UMI_WORKBENCH_CONTEXT_SOURCE_EDITOR;
    record.trigger = UMI_WORKBENCH_CONTEXT_SOURCE_TRIGGER_CARET;
    record.state = UMI_WORKBENCH_CONTEXT_SOURCE_STATE_ACTIVE;
    record.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;

    hash = umi_workbench_context_source_command_hash(&record);
    assert(hash != 0U);

    umi_workbench_context_source_command_touch(
        &record,
        9U,
        1000U);
    assert(record.sequence == 9U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.panel_id, "panel") == 0);
    return 0;
}
