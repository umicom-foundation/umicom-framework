/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/swaption.c
 *
 * PURPOSE:
 *   Represent a vanilla option on a swap rate.
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

#include "umicom/finance/quant/swaption.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_swaption_init(UmiQuantSwaption *record, double annuity, double strike_rate, double forward_swap_rate, int32_t right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (!(annuity >= 0.0 && strike_rate >= 0.0 && forward_swap_rate >= 0.0 && (right == 1 || right == -1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->annuity = annuity;
    record->strike_rate = strike_rate;
    record->forward_swap_rate = forward_swap_rate;
    record->right = right;
    return UMI_STATUS_OK;
}

/* Return annuity-scaled intrinsic swaption value. */
double umi_quant_swaption_intrinsic(const UmiQuantSwaption *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->annuity * fmax(((double)record->right) * (record->forward_swap_rate - record->strike_rate), 0.0);
}
