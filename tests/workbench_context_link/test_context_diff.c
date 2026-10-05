/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_context_diff.c
 *
 * PURPOSE:
 *   Verify the context comparison record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/context_diff.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/context_diff.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkContextDiffTransferEqual(const UmiWorkbenchContextLinkContextDiff *a, const UmiWorkbenchContextLinkContextDiff *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->diff_id, b->diff_id) == 0 &&
        strcmp(a->left_context_id, b->left_context_id) == 0 &&
        strcmp(a->right_context_id, b->right_context_id) == 0 &&
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
static void UmiWorkbenchContextLinkContextDiffTransferTails(UmiWorkbenchContextLinkContextDiff *value)
{
    (void)value;
    {
        size_t used = strlen(value->diff_id) + 1U;
        memset(value->diff_id + used, 0xa5, sizeof(value->diff_id) - used);
    }
    {
        size_t used = strlen(value->left_context_id) + 1U;
        memset(value->left_context_id + used, 0xa5, sizeof(value->left_context_id) - used);
    }
    {
        size_t used = strlen(value->right_context_id) + 1U;
        memset(value->right_context_id + used, 0xa5, sizeof(value->right_context_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkContextDiffTransferMalformed(const UmiWorkbenchContextLinkContextDiff *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkContextDiff invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.diff_id, 'x', sizeof(invalid.diff_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_context_diff_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_context_diff_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated diff_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkContextDiff invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.left_context_id, 'x', sizeof(invalid.left_context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_context_diff_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_context_diff_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated left_context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkContextDiff invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.right_context_id, 'x', sizeof(invalid.right_context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_context_diff_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_context_diff_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated right_context_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkContextDiffTransferCases, UmiWorkbenchContextLinkContextDiff,
    umi_workbench_context_link_context_diff_archive_encode, umi_workbench_context_link_context_diff_archive_decode,
    UmiWorkbenchContextLinkContextDiffTransferEqual, UmiWorkbenchContextLinkContextDiffTransferTails, UmiWorkbenchContextLinkContextDiffTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkContextDiff record;
    UmiWorkbenchContextLinkContextDiff copy;
    uint64_t first_hash;
    umi_workbench_context_link_context_diff_init(&record, "context_diff-id");
    assert(umi_workbench_context_link_context_diff_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkContextDiffTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_context_diff_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_context_diff_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_context_diff_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_context_diff_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_context_diff_hash(&copy) == first_hash);
    umi_workbench_context_link_context_diff_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.diff_id, record.diff_id) == 0);
    return 0;
}
