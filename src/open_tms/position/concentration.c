/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/position/concentration.c
 *
 * PURPOSE:
 *   Apply Position Concentration without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/position/concentration.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_position_concentration(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "position.concentration",
        "Position Concentration",
        UMI_OPEN_TMS_POLICY_UTILISATION,
        0.000000,
        40.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_position_concentration_batch, umi_open_tms_evaluate_position_concentration,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
