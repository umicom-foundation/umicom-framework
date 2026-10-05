/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_conflict.c
 *
 * PURPOSE:
 *   Verify the context-link conflict record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/conflict.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/conflict.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkConflictTransferEqual(const UmiWorkbenchContextLinkConflict *a, const UmiWorkbenchContextLinkConflict *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->conflict_id, b->conflict_id) == 0 &&
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
static void UmiWorkbenchContextLinkConflictTransferTails(UmiWorkbenchContextLinkConflict *value)
{
    (void)value;
    {
        size_t used = strlen(value->conflict_id) + 1U;
        memset(value->conflict_id + used, 0xa5, sizeof(value->conflict_id) - used);
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
static int UmiWorkbenchContextLinkConflictTransferMalformed(const UmiWorkbenchContextLinkConflict *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.conflict_id, 'x', sizeof(invalid.conflict_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated conflict_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkConflict invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_conflict_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_conflict_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkConflictTransferCases, UmiWorkbenchContextLinkConflict,
    umi_workbench_context_link_conflict_archive_encode, umi_workbench_context_link_conflict_archive_decode,
    UmiWorkbenchContextLinkConflictTransferEqual, UmiWorkbenchContextLinkConflictTransferTails, UmiWorkbenchContextLinkConflictTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkConflict record;
    UmiWorkbenchContextLinkConflict copy;
    uint64_t first_hash;
    umi_workbench_context_link_conflict_init(&record, "conflict-id");
    assert(umi_workbench_context_link_conflict_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkConflictTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_conflict_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_conflict_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_conflict_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_conflict_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_conflict_hash(&copy) == first_hash);
    umi_workbench_context_link_conflict_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.conflict_id, record.conflict_id) == 0);
    return 0;
}
