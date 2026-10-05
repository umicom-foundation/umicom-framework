/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_delivery_policy.c
 *
 * PURPOSE:
 *   Verify default delivery pressure policy and validation.
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
#include "umicom/workbench_context_host/delivery_policy.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/workbench_context_host/delivery_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiWorkbenchContextHostDeliveryPolicyTransferEqual(const UmiWorkbenchContextHostDeliveryPolicy *a, const UmiWorkbenchContextHostDeliveryPolicy *b)
{
    return a->max_pending_per_endpoint == b->max_pending_per_endpoint &&
        a->overflow_mode == b->overflow_mode &&
        a->coalesce_same_kind == b->coalesce_same_kind &&
        a->coalesce_same_context == b->coalesce_same_context &&
        a->reject_expired == b->reject_expired &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiWorkbenchContextHostDeliveryPolicyTransferTails(UmiWorkbenchContextHostDeliveryPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiWorkbenchContextHostDeliveryPolicyTransferMalformed(const UmiWorkbenchContextHostDeliveryPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiWorkbenchContextHostDeliveryPolicyTransferCases, UmiWorkbenchContextHostDeliveryPolicy,
    umi_workbench_context_host_delivery_policy_archive_encode, umi_workbench_context_host_delivery_policy_archive_decode,
    UmiWorkbenchContextHostDeliveryPolicyTransferEqual, UmiWorkbenchContextHostDeliveryPolicyTransferTails, UmiWorkbenchContextHostDeliveryPolicyTransferMalformed)

int main(void)
{
    UmiWorkbenchContextHostDeliveryPolicy policy =
        umi_workbench_context_host_delivery_policy_default();
    assert(policy.max_pending_per_endpoint == 32U);
    assert(policy.coalesce_same_context);
    assert(umi_workbench_context_host_delivery_policy_validate(
        &policy) == UMI_STATUS_OK);
    if (UmiWorkbenchContextHostDeliveryPolicyTransferCases(&policy) != 0) return 1;

    policy.max_pending_per_endpoint = 0U;
    assert(umi_workbench_context_host_delivery_policy_validate(
        &policy) == UMI_STATUS_INVALID_ARGUMENT);

    return 0;
}
