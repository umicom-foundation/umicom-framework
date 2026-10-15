/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/dashboard/operations_health.c
 *
 * PURPOSE:
 *   Apply Treasury Operations Health without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/dashboard/operations_health.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_dashboard_operations_health(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "dashboard.operations-health",
        "Treasury Operations Health",
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
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_dashboard_operations_health_batch, umi_open_tms_evaluate_dashboard_operations_health,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
