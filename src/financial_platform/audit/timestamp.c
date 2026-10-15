/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/audit/timestamp.c
 *
 * PURPOSE:
 *   Apply Financial Audit Timestamp without duplicating canonical finance-domain state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/financial_platform/audit/timestamp.h"
#include "../financial_platform_internal.h"

UmiStatus umi_financial_platform_evaluate_audit_timestamp(
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot)
{
    static const UmiFinancialPlatformPolicy policy = {
        "audit.timestamp",
        "Financial Audit Timestamp",
        UMI_FINANCIAL_POLICY_FRESHNESS,
        0.000000,
        86400000.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_financial_platform_evaluate_policy(
        &policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_financial_platform_evaluate_audit_timestamp_batch, umi_financial_platform_evaluate_audit_timestamp,
    UmiFinancialPlatformInput, UmiFinancialPlatformSnapshot)
