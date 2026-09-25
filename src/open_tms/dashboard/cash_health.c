/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/dashboard/cash_health.c
 *
 * PURPOSE:
 *   Apply Cash Dashboard Health without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/dashboard/cash_health.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_dashboard_cash_health(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "dashboard.cash-health",
        "Cash Dashboard Health",
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
