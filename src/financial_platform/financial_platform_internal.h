/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/financial_platform_internal.h
 *
 * PURPOSE:
 *   Share cross-domain financial control-plane policy evaluation while the
 *   canonical finance, payments and accounting engines remain authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_INTERNAL_H
#define UMICOM_FINANCIAL_PLATFORM_INTERNAL_H

#include "umicom/financial_platform/types.h"

typedef enum UmiFinancialPlatformPolicyKind {
    UMI_FINANCIAL_POLICY_READINESS = 1,
    UMI_FINANCIAL_POLICY_COVERAGE,
    UMI_FINANCIAL_POLICY_FRESHNESS,
    UMI_FINANCIAL_POLICY_VALUE,
    UMI_FINANCIAL_POLICY_UTILISATION,
    UMI_FINANCIAL_POLICY_VALIDATION,
    UMI_FINANCIAL_POLICY_CONSISTENCY,
    UMI_FINANCIAL_POLICY_BALANCE,
    UMI_FINANCIAL_POLICY_MATCH,
    UMI_FINANCIAL_POLICY_TOLERANCE,
    UMI_FINANCIAL_POLICY_AGEING,
    UMI_FINANCIAL_POLICY_APPROVAL,
    UMI_FINANCIAL_POLICY_TRANSITION,
    UMI_FINANCIAL_POLICY_COMPLETION,
    UMI_FINANCIAL_POLICY_INTEGRITY,
    UMI_FINANCIAL_POLICY_SEQUENCE,
    UMI_FINANCIAL_POLICY_EXPOSURE,
    UMI_FINANCIAL_POLICY_PNL,
    UMI_FINANCIAL_POLICY_DUE,
    UMI_FINANCIAL_POLICY_LIMIT,
    UMI_FINANCIAL_POLICY_AMOUNT,
    UMI_FINANCIAL_POLICY_HEALTH
} UmiFinancialPlatformPolicyKind;

typedef struct UmiFinancialPlatformPolicy {
    const char *id;
    const char *label;
    UmiFinancialPlatformPolicyKind kind;
    double threshold;
    double warning;
    double weightOne;
    double weightTwo;
    double weightThree;
    double weightFour;
} UmiFinancialPlatformPolicy;

UmiStatus umi_financial_platform_evaluate_policy(
    const UmiFinancialPlatformPolicy *policy,
    const UmiFinancialPlatformInput *input,
    UmiFinancialPlatformSnapshot *outSnapshot);

#endif
