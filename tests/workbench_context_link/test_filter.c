/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_filter.c
 *
 * PURPOSE:
 *   Verify the context-link filter contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/filter.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/filter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkFilterTransferEqual(const UmiWorkbenchContextLinkFilter *a, const UmiWorkbenchContextLinkFilter *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->filter_id, b->filter_id) == 0 &&
        strcmp(a->query_text, b->query_text) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
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
static void UmiWorkbenchContextLinkFilterTransferTails(UmiWorkbenchContextLinkFilter *value)
{
    (void)value;
    {
        size_t used = strlen(value->filter_id) + 1U;
        memset(value->filter_id + used, 0xa5, sizeof(value->filter_id) - used);
    }
    {
        size_t used = strlen(value->query_text) + 1U;
        memset(value->query_text + used, 0xa5, sizeof(value->query_text) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkFilterTransferMalformed(const UmiWorkbenchContextLinkFilter *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.filter_id, 'x', sizeof(invalid.filter_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated filter_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.query_text, 'x', sizeof(invalid.query_text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated query_text was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_filter_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkFilterTransferCases, UmiWorkbenchContextLinkFilter,
    umi_workbench_context_link_filter_archive_encode, umi_workbench_context_link_filter_archive_decode,
    UmiWorkbenchContextLinkFilterTransferEqual, UmiWorkbenchContextLinkFilterTransferTails, UmiWorkbenchContextLinkFilterTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkFilter record;
    UmiWorkbenchContextLinkFilter copy;
    uint64_t first_hash;
    umi_workbench_context_link_filter_init(&record, "filter-id");
    assert(umi_workbench_context_link_filter_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkFilterTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_filter_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_filter_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_filter_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_filter_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_filter_hash(&copy) == first_hash);
    umi_workbench_context_link_filter_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.filter_id, record.filter_id) == 0);
    return 0;
}
