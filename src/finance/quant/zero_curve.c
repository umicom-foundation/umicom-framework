/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/zero_curve.c
 *
 * PURPOSE:
 *   Store and interpolate bounded zero-rate term structures.
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

#include "umicom/finance/quant/zero_curve.h"

#include <math.h>
#include <string.h>

/*
 * Initialise quant zero curve from caller-provided values so later operations receive a
 * known state.
 */
void umi_quant_zero_curve_init(UmiQuantZeroCurve *curve)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (curve != NULL) memset(curve, 0, sizeof *curve);
}

/* Add quant zero curve only after its inputs and available capacity have been checked. */
UmiStatus umi_quant_zero_curve_add(UmiQuantZeroCurve *curve, int32_t tenor_days, double rate)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (curve == NULL || tenor_days < 0 || !umi_quant_number_valid(rate)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (curve->count >= UMI_QUANT_MAX_CURVE_NODES) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (curve->count > 0U && tenor_days <= curve->nodes[curve->count - 1U].tenor_days) return UMI_STATUS_INVALID_STATE;
    curve->nodes[curve->count].tenor_days = tenor_days;
    curve->nodes[curve->count].value = rate;
    curve->count++;
    return UMI_STATUS_OK;
}

/*
 * Provide the quant zero curve rate operation used by this module and its client
 * applications.
 */
UmiStatus umi_quant_zero_curve_rate(const UmiQuantZeroCurve *curve, int32_t tenor_days, double *out_rate)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (curve == NULL || out_rate == NULL || curve->count == 0U || tenor_days < 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (tenor_days <= curve->nodes[0].tenor_days) { *out_rate = curve->nodes[0].value; return UMI_STATUS_OK; }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 1U; i < curve->count; ++i) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (tenor_days <= curve->nodes[i].tenor_days) {
            const double x0 = (double)curve->nodes[i - 1U].tenor_days;
            const double x1 = (double)curve->nodes[i].tenor_days;
            const double w = ((double)tenor_days - x0) / (x1 - x0);
            *out_rate = curve->nodes[i - 1U].value + w * (curve->nodes[i].value - curve->nodes[i - 1U].value);
            return UMI_STATUS_OK;
        }
    }
    *out_rate = curve->nodes[curve->count - 1U].value;
    return UMI_STATUS_OK;
}
