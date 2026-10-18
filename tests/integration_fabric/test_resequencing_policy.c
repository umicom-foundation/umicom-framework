/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_resequencing_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the resequencing policy Integration Fabric capability.
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
#include "umicom/integration/fabric/resequencing_policy.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/resequencing_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricResequencingPolicyTransferEqual(const UmiFabricResequencingPolicy *a, const UmiFabricResequencingPolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        a->maximum_buffered == b->maximum_buffered &&
        a->gap_timeout_ms == b->gap_timeout_ms &&
        a->release_on_timeout == b->release_on_timeout;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricResequencingPolicyTransferTails(UmiFabricResequencingPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricResequencingPolicyTransferMalformed(const UmiFabricResequencingPolicy *sample)
{
    (void)sample;
    {
        UmiFabricResequencingPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_resequencing_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_resequencing_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricResequencingPolicyTransferCases, UmiFabricResequencingPolicy,
    umi_fabric_resequencing_policy_archive_encode, umi_fabric_resequencing_policy_archive_decode,
    UmiFabricResequencingPolicyTransferEqual, UmiFabricResequencingPolicyTransferTails, UmiFabricResequencingPolicyTransferMalformed)

/* Exercise the caller-workspace API using this owner\'s real fixture and field comparator. */
#include "../value_archive/workspace_cases.h"
UMI_TEST_VALUE_WORKSPACE(UmiFabricResequencingPolicyTransferCasesWithWorkspace, UmiFabricResequencingPolicy,
    umi_fabric_resequencing_policy_archive_encode, umi_fabric_resequencing_policy_archive_decode_with_workspace, UmiFabricResequencingPolicyTransferEqual)

#include "../value_archive/conditional_cases.h"
UMI_TEST_CONDITIONAL_ARCHIVE(UmiFabricResequencingPolicyConditionalArchiveCases, UmiFabricResequencingPolicy,
    umi_fabric_resequencing_policy_archive_encode, umi_fabric_resequencing_policy_archive_decode_if_unchanged,
    UmiFabricResequencingPolicyTransferEqual, UmiFabricResequencingPolicyTransferTails, UmiFabricResequencingPolicyTransferMalformed)



/* A rejected replacement must preserve the previously accepted resequencing policy value. */
static int UmiFabricResequencingPolicyConstructionCases(void)
{
    UmiFabricResequencingPolicy *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_fabric_resequencing_policy_init(value, "seq", 128U, 500U, true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->policy_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_resequencing_policy_init(value, oversized, 128U, 500U, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_fabric_resequencing_policy_init(value, value->policy_id, 128U, 500U, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->policy_id, "seq") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiFabricResequencingPolicyConstructionCases() != 0) return 1;

    UmiFabricResequencingPolicy item;
    CHECK(umi_fabric_resequencing_policy_init(&item,"seq",128U,500U,true)==UMI_STATUS_OK);
    if (UmiFabricResequencingPolicyTransferCases(&item) != 0) return 1;
    if (UmiFabricResequencingPolicyTransferCasesWithWorkspace(&item) != 0) return 1;
    if (UmiFabricResequencingPolicyConditionalArchiveCases(&item) != 0) return 1;

    CHECK(item.release_on_timeout);
    return 0;
}
