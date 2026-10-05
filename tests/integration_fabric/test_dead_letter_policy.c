/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_dead_letter_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the dead letter policy Integration Fabric capability.
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
#include "umicom/integration/fabric/dead_letter_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/dead_letter_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricDeadLetterPolicyTransferEqual(const UmiFabricDeadLetterPolicy *a, const UmiFabricDeadLetterPolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        strcmp(a->destination, b->destination) == 0 &&
        a->after_attempts == b->after_attempts &&
        a->retention_ms == b->retention_ms &&
        a->include_payload == b->include_payload;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricDeadLetterPolicyTransferTails(UmiFabricDeadLetterPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
    {
        size_t used = strlen(value->destination) + 1U;
        memset(value->destination + used, 0xa5, sizeof(value->destination) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricDeadLetterPolicyTransferMalformed(const UmiFabricDeadLetterPolicy *sample)
{
    (void)sample;
    {
        UmiFabricDeadLetterPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_dead_letter_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_dead_letter_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiFabricDeadLetterPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.destination, 'x', sizeof(invalid.destination));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_dead_letter_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_dead_letter_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated destination was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricDeadLetterPolicyTransferCases, UmiFabricDeadLetterPolicy,
    umi_fabric_dead_letter_policy_archive_encode, umi_fabric_dead_letter_policy_archive_decode,
    UmiFabricDeadLetterPolicyTransferEqual, UmiFabricDeadLetterPolicyTransferTails, UmiFabricDeadLetterPolicyTransferMalformed)

int main(void) {
    UmiFabricDeadLetterPolicy item;
    CHECK(umi_fabric_dead_letter_policy_init(&item,"dlq","topic://dead",3U,60000U,false)==UMI_STATUS_OK);
    if (UmiFabricDeadLetterPolicyTransferCases(&item) != 0) return 1;

    CHECK(item.after_attempts==3U);
    return 0;
}
