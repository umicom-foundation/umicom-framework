/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_inspector_row.c
 *
 * PURPOSE:
 *   Verify the context inspector row contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/inspector_row.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/inspector_row.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkInspectorRowTransferEqual(const UmiWorkbenchContextLinkInspectorRow *a, const UmiWorkbenchContextLinkInspectorRow *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->row_id, b->row_id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->value_text, b->value_text) == 0 &&
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
static void UmiWorkbenchContextLinkInspectorRowTransferTails(UmiWorkbenchContextLinkInspectorRow *value)
{
    (void)value;
    {
        size_t used = strlen(value->row_id) + 1U;
        memset(value->row_id + used, 0xa5, sizeof(value->row_id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->value_text) + 1U;
        memset(value->value_text + used, 0xa5, sizeof(value->value_text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkInspectorRowTransferMalformed(const UmiWorkbenchContextLinkInspectorRow *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkInspectorRow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.row_id, 'x', sizeof(invalid.row_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_inspector_row_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_inspector_row_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated row_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkInspectorRow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_inspector_row_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_inspector_row_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkInspectorRow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value_text, 'x', sizeof(invalid.value_text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_inspector_row_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_inspector_row_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value_text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkInspectorRowTransferCases, UmiWorkbenchContextLinkInspectorRow,
    umi_workbench_context_link_inspector_row_archive_encode, umi_workbench_context_link_inspector_row_archive_decode,
    UmiWorkbenchContextLinkInspectorRowTransferEqual, UmiWorkbenchContextLinkInspectorRowTransferTails, UmiWorkbenchContextLinkInspectorRowTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiWorkbenchContextLinkInspectorRowTransferCasesWithWorkspace, UmiWorkbenchContextLinkInspectorRow,
    umi_workbench_context_link_inspector_row_archive_encode, umi_workbench_context_link_inspector_row_archive_decode_with_workspace, UmiWorkbenchContextLinkInspectorRowTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiWorkbenchContextLinkInspectorRowConditionalArchiveCases, UmiWorkbenchContextLinkInspectorRow,
    umi_workbench_context_link_inspector_row_archive_encode, umi_workbench_context_link_inspector_row_archive_decode_if_unchanged,
    UmiWorkbenchContextLinkInspectorRowTransferEqual, UmiWorkbenchContextLinkInspectorRowTransferTails, UmiWorkbenchContextLinkInspectorRowTransferMalformed)


/* Delayed context edits must preserve newer work and reject exhausted tokens. */
#include "../record_integrity/reviewed_value_cases.h"
UMI_TEST_REVIEWED_VALUE(UmiWorkbenchContextLinkInspectorRowReviewedCases, UmiWorkbenchContextLinkInspectorRow,
    row_id, value_text, umi_workbench_context_link_inspector_row_replace_if_current, UmiWorkbenchContextLinkInspectorRowTransferEqual)
static int UmiWorkbenchContextLinkInspectorRowMutationGuards(const UmiWorkbenchContextLinkInspectorRow *sample)
{
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextLinkInspectorRow, label, umi_workbench_context_link_inspector_row_set_primary, sample);
    UMI_TEST_REVIEWED_TEXT_FIELD(UmiWorkbenchContextLinkInspectorRow, value_text, umi_workbench_context_link_inspector_row_set_secondary, sample);
    UmiWorkbenchContextLinkInspectorRow value = *sample;
    unsigned char before[sizeof(value)];
    value.revision = UINT64_MAX - 1U;
    umi_workbench_context_link_inspector_row_touch(&value, 33U, 44U);
    if (value.revision != UINT64_MAX || value.sequence != 33U ||
        value.timestamp_ms != 44U) return 1;
    memcpy(before, &value, sizeof(value));
    umi_workbench_context_link_inspector_row_touch(&value, 55U, 66U);
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

int main(void)
{
    UmiWorkbenchContextLinkInspectorRow record;
    UmiWorkbenchContextLinkInspectorRow copy;
    uint64_t first_hash;
    umi_workbench_context_link_inspector_row_init(&record, "inspector_row-id");
    assert(umi_workbench_context_link_inspector_row_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkInspectorRowTransferCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkInspectorRowTransferCasesWithWorkspace(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkInspectorRowConditionalArchiveCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkInspectorRowReviewedCases(&record) != 0) return 1;
    if (UmiWorkbenchContextLinkInspectorRowMutationGuards(&record) != 0) return 1;

    assert(umi_workbench_context_link_inspector_row_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_inspector_row_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_inspector_row_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_inspector_row_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_inspector_row_hash(&copy) == first_hash);
    umi_workbench_context_link_inspector_row_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.row_id, record.row_id) == 0);
    return 0;
}
