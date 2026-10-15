/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/risk/readiness.c
 *
 * PURPOSE:
 *   Apply Risk Readiness without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/risk/readiness.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_risk_readiness(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "risk.readiness",
        "Risk Readiness",
        UMI_OPEN_TMS_POLICY_HEALTH,
        90.000000,
        70.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_risk_readiness_batch, umi_open_tms_evaluate_risk_readiness,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
