/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/approval_policy.h
 *
 * PURPOSE:
 *   Model maker-checker approval evidence and segregation-of-duties readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_APPROVAL_POLICY_H
#define UMICOM_FINANCE_PAYMENTS_APPROVAL_POLICY_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments approval policy data shared with callers of this public contract.
 */
typedef struct UmiPaymentsApprovalPolicy {
    UmiFinancialId id;
    uint32_t makers_required;
    uint32_t makers_received;
    uint32_t checkers_required;
    uint32_t checkers_received;
    bool segregated;
} UmiPaymentsApprovalPolicy;
/**
 * Initialise payments approval policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_approval_policy_init(UmiPaymentsApprovalPolicy *value,
    const char *id,
    uint32_t makers_required,
    uint32_t makers_received,
    uint32_t checkers_required,
    uint32_t checkers_received,
    bool segregated);
/**
 * Check that payments approval policy satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_approval_policy_valid(const UmiPaymentsApprovalPolicy *value);
/**
 * Provide the payments approval policy satisfied operation used by this module and its
 * client applications.
 */
bool umi_payments_approval_policy_satisfied(const UmiPaymentsApprovalPolicy *value);
#ifdef __cplusplus
}
#endif
#endif
