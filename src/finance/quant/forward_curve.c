/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/forward_curve.c
 *
 * PURPOSE:
 *   Derive simple-compounded forward rates between zero-curve tenors.
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

#include "umicom/finance/quant/forward_curve.h"

#include <math.h>
#include <string.h>

/*
 * Provide the quant forward curve rate operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_forward_curve_rate(double near_zero, int32_t near_days, double far_zero, int32_t far_days, double *out_forward)
{
    double near_df, far_df, dt;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_forward == NULL || near_days < 0 || far_days <= near_days) return UMI_STATUS_INVALID_ARGUMENT;
    near_df = exp(-near_zero * ((double)near_days / 365.0));
    far_df = exp(-far_zero * ((double)far_days / 365.0));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (far_df <= 0.0) return UMI_STATUS_INVALID_STATE;
    dt = (double)(far_days - near_days) / 365.0;
    *out_forward = (near_df / far_df - 1.0) / dt;
    return UMI_STATUS_OK;
}
