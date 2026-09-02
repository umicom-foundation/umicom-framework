/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/greek_set.c
 *
 * PURPOSE:
 *   Store first-order and second-order option sensitivity evidence.
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

#include "umicom/finance/quant/greek_set.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_greek_set_init(UmiQuantGreekSet *record, double delta, double gamma, double vega, double theta, double rho)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(umi_quant_number_valid(delta) && umi_quant_number_valid(gamma) && umi_quant_number_valid(vega) && umi_quant_number_valid(theta) && umi_quant_number_valid(rho))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->delta = delta;
    record->gamma = gamma;
    record->vega = vega;
    record->theta = theta;
    record->rho = rho;
    return UMI_STATUS_OK;
}

/* Return aggregate absolute first-order risk magnitude. */
double umi_quant_greek_set_absolute_first_order(const UmiQuantGreekSet *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return fabs(record->delta) + fabs(record->vega) + fabs(record->theta) + fabs(record->rho);
}
