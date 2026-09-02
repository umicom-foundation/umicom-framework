/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/fx_forward.c
 *
 * PURPOSE:
 *   Model an FX forward using spot, forward points and maturity.
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

#include "umicom/finance/quant/fx_forward.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_fx_forward_init(UmiQuantFxForward *record, double spot, double forward_points, int32_t maturity_days)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!(umi_quant_number_valid(spot) && spot > 0.0 && umi_quant_number_valid(forward_points) && maturity_days > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->spot = spot;
    record->forward_points = forward_points;
    record->maturity_days = maturity_days;
    return UMI_STATUS_OK;
}

/* Return the outright forward rate represented by spot plus forward points. */
double umi_quant_fx_forward_outright(const UmiQuantFxForward *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (record == NULL) return 0.0;
    return record->spot + record->forward_points;
}
