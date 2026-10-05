/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_metric.c
 *
 * PURPOSE:
 *   Verify the context-link metric sample contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/metric.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/metric.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkMetricTransferEqual(const UmiWorkbenchContextLinkMetric *a, const UmiWorkbenchContextLinkMetric *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->metric_id, b->metric_id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        strcmp(a->unit, b->unit) == 0 &&
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
static void UmiWorkbenchContextLinkMetricTransferTails(UmiWorkbenchContextLinkMetric *value)
{
    (void)value;
    {
        size_t used = strlen(value->metric_id) + 1U;
        memset(value->metric_id + used, 0xa5, sizeof(value->metric_id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
    {
        size_t used = strlen(value->unit) + 1U;
        memset(value->unit + used, 0xa5, sizeof(value->unit) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkMetricTransferMalformed(const UmiWorkbenchContextLinkMetric *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkMetric invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.metric_id, 'x', sizeof(invalid.metric_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_metric_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_metric_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated metric_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkMetric invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_metric_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_metric_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkMetric invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.unit, 'x', sizeof(invalid.unit));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_metric_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_metric_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated unit was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkMetricTransferCases, UmiWorkbenchContextLinkMetric,
    umi_workbench_context_link_metric_archive_encode, umi_workbench_context_link_metric_archive_decode,
    UmiWorkbenchContextLinkMetricTransferEqual, UmiWorkbenchContextLinkMetricTransferTails, UmiWorkbenchContextLinkMetricTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkMetric record;
    UmiWorkbenchContextLinkMetric copy;
    uint64_t first_hash;
    umi_workbench_context_link_metric_init(&record, "metric-id");
    assert(umi_workbench_context_link_metric_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkMetricTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_metric_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_metric_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_metric_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_metric_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_metric_hash(&copy) == first_hash);
    umi_workbench_context_link_metric_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.metric_id, record.metric_id) == 0);
    return 0;
}
