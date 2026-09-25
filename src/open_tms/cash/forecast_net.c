/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/cash/forecast_net.c
 *
 * PURPOSE:
 *   Apply Cash Forecast Net without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/cash/forecast_net.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_cash_forecast_net(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "cash.forecast-net",
        "Cash Forecast Net",
        UMI_OPEN_TMS_POLICY_CASH_NET,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}
