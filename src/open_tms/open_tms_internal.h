/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/open_tms_internal.h
 *
 * PURPOSE:
 *   Share TMS control calculations while canonical finance/treasury engines
 *   remain authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OPEN_TMS_INTERNAL_H
#define UMICOM_OPEN_TMS_INTERNAL_H

#include "umicom/open_tms/types.h"

typedef enum UmiOpenTmsPolicyKind {
    UMI_OPEN_TMS_POLICY_READINESS = 1,
    UMI_OPEN_TMS_POLICY_VALUE,
    UMI_OPEN_TMS_POLICY_AMOUNT,
    UMI_OPEN_TMS_POLICY_COVERAGE,
    UMI_OPEN_TMS_POLICY_FRESHNESS,
    UMI_OPEN_TMS_POLICY_LIMIT,
    UMI_OPEN_TMS_POLICY_UTILISATION,
    UMI_OPEN_TMS_POLICY_CASH_NET,
    UMI_OPEN_TMS_POLICY_SHORTFALL,
    UMI_OPEN_TMS_POLICY_SURPLUS,
    UMI_OPEN_TMS_POLICY_LIQUIDITY_GAP,
    UMI_OPEN_TMS_POLICY_LIQUIDITY_BUFFER,
    UMI_OPEN_TMS_POLICY_RATIO,
    UMI_OPEN_TMS_POLICY_SCORE,
    UMI_OPEN_TMS_POLICY_HEALTH,
    UMI_OPEN_TMS_POLICY_FORWARD_RATE,
    UMI_OPEN_TMS_POLICY_FORWARD_POINTS,
    UMI_OPEN_TMS_POLICY_DISCOUNT,
    UMI_OPEN_TMS_POLICY_PV,
    UMI_OPEN_TMS_POLICY_ACCRUAL,
    UMI_OPEN_TMS_POLICY_PNL,
    UMI_OPEN_TMS_POLICY_EXPOSURE,
    UMI_OPEN_TMS_POLICY_RISK_SCORE,
    UMI_OPEN_TMS_POLICY_BREACH,
    UMI_OPEN_TMS_POLICY_DUE,
    UMI_OPEN_TMS_POLICY_TRANSITION,
    UMI_OPEN_TMS_POLICY_APPROVAL,
    UMI_OPEN_TMS_POLICY_MATCH,
    UMI_OPEN_TMS_POLICY_COMPLETION,
    UMI_OPEN_TMS_POLICY_CONSISTENCY,
    UMI_OPEN_TMS_POLICY_BALANCE
} UmiOpenTmsPolicyKind;

typedef struct UmiOpenTmsPolicy {
    const char *id;
    const char *label;
    UmiOpenTmsPolicyKind kind;
    double threshold;
    double warning;
    double weightOne;
    double weightTwo;
    double weightThree;
    double weightFour;
} UmiOpenTmsPolicy;

UmiStatus umi_open_tms_evaluate_policy(
    const UmiOpenTmsPolicy *policy,
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot);

#endif
