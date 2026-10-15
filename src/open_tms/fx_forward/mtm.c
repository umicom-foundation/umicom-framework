/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/fx_forward/mtm.c
 *
 * PURPOSE:
 *   Apply FX Forward Mark-to-Market without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/fx_forward/mtm.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_fx_forward_mtm(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "fx_forward.mtm",
        "FX Forward Mark-to-Market",
        UMI_OPEN_TMS_POLICY_PNL,
        0.000000,
        0.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_fx_forward_mtm_batch, umi_open_tms_evaluate_fx_forward_mtm,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
