/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_recent_context.c
 *
 * PURPOSE:
 *   Verify the recent context record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/recent_context.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/recent_context.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkRecentContextTransferEqual(const UmiWorkbenchContextLinkRecentContext *a, const UmiWorkbenchContextLinkRecentContext *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->recent_id, b->recent_id) == 0 &&
        strcmp(a->group_id, b->group_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
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
static void UmiWorkbenchContextLinkRecentContextTransferTails(UmiWorkbenchContextLinkRecentContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->recent_id) + 1U;
        memset(value->recent_id + used, 0xa5, sizeof(value->recent_id) - used);
    }
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkRecentContextTransferMalformed(const UmiWorkbenchContextLinkRecentContext *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.recent_id, 'x', sizeof(invalid.recent_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated recent_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkRecentContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_recent_context_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_recent_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkRecentContextTransferCases, UmiWorkbenchContextLinkRecentContext,
    umi_workbench_context_link_recent_context_archive_encode, umi_workbench_context_link_recent_context_archive_decode,
    UmiWorkbenchContextLinkRecentContextTransferEqual, UmiWorkbenchContextLinkRecentContextTransferTails, UmiWorkbenchContextLinkRecentContextTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkRecentContext record;
    UmiWorkbenchContextLinkRecentContext copy;
    uint64_t first_hash;
    umi_workbench_context_link_recent_context_init(&record, "recent_context-id");
    assert(umi_workbench_context_link_recent_context_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkRecentContextTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_recent_context_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_recent_context_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_recent_context_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_recent_context_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_recent_context_hash(&copy) == first_hash);
    umi_workbench_context_link_recent_context_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.recent_id, record.recent_id) == 0);
    return 0;
}
