/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/approval_policy.h
 *
 * PURPOSE:
 *   Model maker-checker approval evidence and segregation-of-duties readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_APPROVAL_POLICY_H
#define UMICOM_FINANCE_PAYMENTS_APPROVAL_POLICY_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiPaymentsApprovalPolicy {
    UmiFinancialId id;
    uint32_t makers_required;
    uint32_t makers_received;
    uint32_t checkers_required;
    uint32_t checkers_received;
    bool segregated;
} UmiPaymentsApprovalPolicy;
UmiStatus umi_payments_approval_policy_init(UmiPaymentsApprovalPolicy *value,
    const char *id,
    uint32_t makers_required,
    uint32_t makers_received,
    uint32_t checkers_required,
    uint32_t checkers_received,
    bool segregated);
bool umi_payments_approval_policy_valid(const UmiPaymentsApprovalPolicy *value);
bool umi_payments_approval_policy_satisfied(const UmiPaymentsApprovalPolicy *value);
#ifdef __cplusplus
}
#endif
#endif
