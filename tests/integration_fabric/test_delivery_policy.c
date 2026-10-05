/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/integration_fabric/test_delivery_policy.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the delivery policy Integration Fabric capability.
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
#include "umicom/integration/fabric/delivery_policy.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr,"CHECK failed: %s:%d: %s\n",__FILE__,__LINE__,#expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/integration/fabric/delivery_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFabricDeliveryPolicyTransferEqual(const UmiFabricDeliveryPolicy *a, const UmiFabricDeliveryPolicy *b)
{
    return strcmp(a->policy_id, b->policy_id) == 0 &&
        a->mode == b->mode &&
        a->max_attempts == b->max_attempts &&
        a->acknowledgement_timeout_ms == b->acknowledgement_timeout_ms &&
        a->durable == b->durable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFabricDeliveryPolicyTransferTails(UmiFabricDeliveryPolicy *value)
{
    (void)value;
    {
        size_t used = strlen(value->policy_id) + 1U;
        memset(value->policy_id + used, 0xa5, sizeof(value->policy_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFabricDeliveryPolicyTransferMalformed(const UmiFabricDeliveryPolicy *sample)
{
    (void)sample;
    {
        UmiFabricDeliveryPolicy invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.policy_id, 'x', sizeof(invalid.policy_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_fabric_delivery_policy_validate(&invalid) != UMI_STATUS_OK) ||
            umi_fabric_delivery_policy_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated policy_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFabricDeliveryPolicyTransferCases, UmiFabricDeliveryPolicy,
    umi_fabric_delivery_policy_archive_encode, umi_fabric_delivery_policy_archive_decode,
    UmiFabricDeliveryPolicyTransferEqual, UmiFabricDeliveryPolicyTransferTails, UmiFabricDeliveryPolicyTransferMalformed)

int main(void) {
    UmiFabricDeliveryPolicy item;
    CHECK(umi_fabric_delivery_policy_init(&item,"orders",UMI_FABRIC_DELIVERY_AT_LEAST_ONCE,3U,5000U,true)==UMI_STATUS_OK);
    if (UmiFabricDeliveryPolicyTransferCases(&item) != 0) return 1;

    CHECK(item.durable);
    return 0;
}
