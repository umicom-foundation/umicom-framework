/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_statistics.c
 *
 * PURPOSE:
 *   Verify the context-link statistics record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/statistics.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/statistics.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkStatisticsTransferEqual(const UmiWorkbenchContextLinkStatistics *a, const UmiWorkbenchContextLinkStatistics *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->statistics_id, b->statistics_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->scope_id, b->scope_id) == 0 &&
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
static void UmiWorkbenchContextLinkStatisticsTransferTails(UmiWorkbenchContextLinkStatistics *value)
{
    (void)value;
    {
        size_t used = strlen(value->statistics_id) + 1U;
        memset(value->statistics_id + used, 0xa5, sizeof(value->statistics_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->scope_id) + 1U;
        memset(value->scope_id + used, 0xa5, sizeof(value->scope_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkStatisticsTransferMalformed(const UmiWorkbenchContextLinkStatistics *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkStatistics invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.statistics_id, 'x', sizeof(invalid.statistics_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_statistics_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_statistics_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated statistics_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkStatistics invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_statistics_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_statistics_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkStatistics invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.scope_id, 'x', sizeof(invalid.scope_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_statistics_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_statistics_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated scope_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkStatisticsTransferCases, UmiWorkbenchContextLinkStatistics,
    umi_workbench_context_link_statistics_archive_encode, umi_workbench_context_link_statistics_archive_decode,
    UmiWorkbenchContextLinkStatisticsTransferEqual, UmiWorkbenchContextLinkStatisticsTransferTails, UmiWorkbenchContextLinkStatisticsTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkStatistics record;
    UmiWorkbenchContextLinkStatistics copy;
    uint64_t first_hash;
    umi_workbench_context_link_statistics_init(&record, "statistics-id");
    assert(umi_workbench_context_link_statistics_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkStatisticsTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_statistics_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_statistics_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_statistics_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_statistics_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_statistics_hash(&copy) == first_hash);
    umi_workbench_context_link_statistics_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.statistics_id, record.statistics_id) == 0);
    return 0;
}
