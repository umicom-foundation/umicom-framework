/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/fx_swap.c
 *
 * PURPOSE:
 *   Model an FX swap using near and far forward rates.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/fx_swap.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_fx_swap_init(UmiQuantFxSwap *record, double near_rate, double far_rate, int32_t far_days)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(umi_quant_number_valid(near_rate) && near_rate > 0.0 && umi_quant_number_valid(far_rate) && far_rate > 0.0 && far_days > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->near_rate = near_rate;
    record->far_rate = far_rate;
    record->far_days = far_days;
    return UMI_STATUS_OK;
}

/* Return the far-minus-near swap points. */
double umi_quant_fx_swap_swap_points(const UmiQuantFxSwap *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->far_rate - record->near_rate;
}
