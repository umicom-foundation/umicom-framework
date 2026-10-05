/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_route_hop.c
 *
 * PURPOSE:
 *   Verify the context route hop contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/route_hop.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/route_hop.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkRouteHopTransferEqual(const UmiWorkbenchContextLinkRouteHop *a, const UmiWorkbenchContextLinkRouteHop *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->hop_id, b->hop_id) == 0 &&
        strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->destination_id, b->destination_id) == 0 &&
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
static void UmiWorkbenchContextLinkRouteHopTransferTails(UmiWorkbenchContextLinkRouteHop *value)
{
    (void)value;
    {
        size_t used = strlen(value->hop_id) + 1U;
        memset(value->hop_id + used, 0xa5, sizeof(value->hop_id) - used);
    }
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->destination_id) + 1U;
        memset(value->destination_id + used, 0xa5, sizeof(value->destination_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkRouteHopTransferMalformed(const UmiWorkbenchContextLinkRouteHop *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkRouteHop invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.hop_id, 'x', sizeof(invalid.hop_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_route_hop_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_route_hop_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated hop_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkRouteHop invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_route_hop_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_route_hop_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkRouteHop invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination_id, 'x', sizeof(invalid.destination_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_route_hop_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_route_hop_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkRouteHopTransferCases, UmiWorkbenchContextLinkRouteHop,
    umi_workbench_context_link_route_hop_archive_encode, umi_workbench_context_link_route_hop_archive_decode,
    UmiWorkbenchContextLinkRouteHopTransferEqual, UmiWorkbenchContextLinkRouteHopTransferTails, UmiWorkbenchContextLinkRouteHopTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkRouteHop record;
    UmiWorkbenchContextLinkRouteHop copy;
    uint64_t first_hash;
    umi_workbench_context_link_route_hop_init(&record, "route_hop-id");
    assert(umi_workbench_context_link_route_hop_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkRouteHopTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_route_hop_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_route_hop_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_route_hop_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_route_hop_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_route_hop_hash(&copy) == first_hash);
    umi_workbench_context_link_route_hop_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.hop_id, record.hop_id) == 0);
    return 0;
}
