/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_export_record.c
 *
 * PURPOSE:
 *   Verify the context-link export record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/export_record.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/export_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkExportRecordTransferEqual(const UmiWorkbenchContextLinkExportRecord *a, const UmiWorkbenchContextLinkExportRecord *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->record_id, b->record_id) == 0 &&
        strcmp(a->destination_uri, b->destination_uri) == 0 &&
        strcmp(a->workspace_id, b->workspace_id) == 0 &&
        a->context_kind == b->context_kind &&
        a->colour == b->colour &&
        a->mode == b->mode &&
        a->state == b->state &&
        a->origin == b->origin &&
        a->priority == b->priority &&
        a->flags == b->flags &&
        a->sequence == b->sequence &&
        a->timestamp_ms == b->timestamp_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextLinkExportRecordTransferTails(UmiWorkbenchContextLinkExportRecord *value)
{
    (void)value;
    {
        size_t used = strlen(value->record_id) + 1U;
        memset(value->record_id + used, 0xa5, sizeof(value->record_id) - used);
    }
    {
        size_t used = strlen(value->destination_uri) + 1U;
        memset(value->destination_uri + used, 0xa5, sizeof(value->destination_uri) - used);
    }
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkExportRecordTransferMalformed(const UmiWorkbenchContextLinkExportRecord *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkExportRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.record_id, 'x', sizeof(invalid.record_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_export_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_export_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated record_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkExportRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination_uri, 'x', sizeof(invalid.destination_uri));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_export_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_export_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination_uri was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkExportRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_export_record_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_export_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkExportRecordTransferCases, UmiWorkbenchContextLinkExportRecord,
    umi_workbench_context_link_export_record_archive_encode, umi_workbench_context_link_export_record_archive_decode,
    UmiWorkbenchContextLinkExportRecordTransferEqual, UmiWorkbenchContextLinkExportRecordTransferTails, UmiWorkbenchContextLinkExportRecordTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkExportRecord record;
    UmiWorkbenchContextLinkExportRecord copy;
    uint64_t first_hash;
    umi_workbench_context_link_export_record_init(&record, "export_record-id");
    assert(umi_workbench_context_link_export_record_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkExportRecordTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_export_record_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_export_record_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_export_record_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_export_record_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_export_record_hash(&copy) == first_hash);
    umi_workbench_context_link_export_record_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.record_id, record.record_id) == 0);
    return 0;
}
