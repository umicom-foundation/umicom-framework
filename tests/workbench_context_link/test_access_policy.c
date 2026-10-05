/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_access_policy.c
 *
 * PURPOSE:
 *   Verify the context-link access policy contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/access_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_link/access_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextLinkAccessPolicyTransferEqual(const UmiWorkbenchContextLinkAccessPolicy *a, const UmiWorkbenchContextLinkAccessPolicy *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->policy_id, b->policy_id) == 0 &&
        strcmp(a->subject_id, b->subject_id) == 0 &&
        strcmp(a->resource_id, b->resource_id) == 0 &&
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
static void UmiWorkbenchContextLinkAccessPolicyTransferTails(UmiWorkbenchContextLinkAccessPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
    {
        size_t used = strlen(value->subject_id) + 1U;
        memset(value->subject_id + used, 0xa5, sizeof(value->subject_id) - used);
    }
    {
        size_t used = strlen(value->resource_id) + 1U;
        memset(value->resource_id + used, 0xa5, sizeof(value->resource_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextLinkAccessPolicyTransferMalformed(const UmiWorkbenchContextLinkAccessPolicy *sample)
{
    (void)sample;
    {
        UmiWorkbenchContextLinkAccessPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_access_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_access_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkAccessPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subject_id, 'x', sizeof(invalid.subject_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_access_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_access_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subject_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiWorkbenchContextLinkAccessPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resource_id, 'x', sizeof(invalid.resource_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_workbench_context_link_access_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_workbench_context_link_access_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resource_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextLinkAccessPolicyTransferCases, UmiWorkbenchContextLinkAccessPolicy,
    umi_workbench_context_link_access_policy_archive_encode, umi_workbench_context_link_access_policy_archive_decode,
    UmiWorkbenchContextLinkAccessPolicyTransferEqual, UmiWorkbenchContextLinkAccessPolicyTransferTails, UmiWorkbenchContextLinkAccessPolicyTransferMalformed)

int main(void)
{
    UmiWorkbenchContextLinkAccessPolicy record;
    UmiWorkbenchContextLinkAccessPolicy copy;
    uint64_t first_hash;
    umi_workbench_context_link_access_policy_init(&record, "access_policy-id");
    assert(umi_workbench_context_link_access_policy_validate(&record) == UMI_STATUS_OK);
    if (UmiWorkbenchContextLinkAccessPolicyTransferCases(&record) != 0) return 1;

    assert(umi_workbench_context_link_access_policy_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_access_policy_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_access_policy_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_access_policy_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_access_policy_hash(&copy) == first_hash);
    umi_workbench_context_link_access_policy_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.policy_id, record.policy_id) == 0);
    return 0;
}
