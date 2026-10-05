/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_instrument_item.c
 *
 * PURPOSE:
 *   Verify the structured instrument item contract, mutation and hashing.
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

#include "umicom/workbench_selection/instrument_item.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_selection/instrument_item.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchSelectionInstrumentItemTransferEqual(const UmiWorkbenchSelectionInstrumentItem *a, const UmiWorkbenchSelectionInstrumentItem *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->record_id, b->record_id) == 0 &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        strcmp(a->secondary_id, b->secondary_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        a->selection_kind == b->selection_kind &&
        a->activation == b->activation &&
        a->state == b->state &&
        a->context_kind == b->context_kind &&
        a->flags == b->flags &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchSelectionInstrumentItemTransferTails(UmiWorkbenchSelectionInstrumentItem *value)
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
        size_t used = strlen(value->secondary_id) + 1U;
        memset(value->secondary_id + used, 0xa5, sizeof(value->secondary_id) - used);
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
static int UmiWorkbenchSelectionInstrumentItemTransferMalformed(const UmiWorkbenchSelectionInstrumentItem *sample)
{
    (void)sample;
    {
        UmiWorkbenchSelectionInstrumentItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.record_id, 'x', sizeof(invalid.record_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_instrument_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_instrument_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated record_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionInstrumentItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_instrument_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_instrument_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionInstrumentItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_instrument_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_instrument_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionInstrumentItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.secondary_id, 'x', sizeof(invalid.secondary_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_instrument_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_instrument_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated secondary_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionInstrumentItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_instrument_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_instrument_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchSelectionInstrumentItem invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_selection_instrument_item_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_selection_instrument_item_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchSelectionInstrumentItemTransferCases, UmiWorkbenchSelectionInstrumentItem,
    umi_workbench_selection_instrument_item_archive_encode, umi_workbench_selection_instrument_item_archive_decode,
    UmiWorkbenchSelectionInstrumentItemTransferEqual, UmiWorkbenchSelectionInstrumentItemTransferTails, UmiWorkbenchSelectionInstrumentItemTransferMalformed)

int main(void)
{
    UmiWorkbenchSelectionInstrumentItem record;
    uint64_t hash;

    umi_workbench_selection_instrument_item_init(
        &record,
        "instrument_item-record");
    assert(umi_workbench_selection_instrument_item_validate(
        &record) == UMI_STATUS_OK);
    if (UmiWorkbenchSelectionInstrumentItemTransferCases(&record) != 0) return 1;

    assert(umi_workbench_selection_instrument_item_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_instrument_item_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_instrument_item_set_secondary(
        &record, "secondary") == UMI_STATUS_OK);
    assert(umi_workbench_selection_instrument_item_set_group(
        &record, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_selection_instrument_item_set_label(
        &record, "label") == UMI_STATUS_OK);

    record.selection_kind = UMI_WORKBENCH_SELECTION_PROJECT;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.activation = UMI_WORKBENCH_SELECTION_ACTIVATION_OPEN;
    record.state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;

    hash = umi_workbench_selection_instrument_item_hash(&record);
    assert(hash != 0U);
    umi_workbench_selection_instrument_item_touch(
        &record, 7U, 1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    return 0;
}
