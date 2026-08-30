/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/cap_floor.c
 *
 * PURPOSE:
 *   Represent a single caplet or floorlet payoff.
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

#include "umicom/finance/quant/cap_floor.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_cap_floor_init(UmiQuantCapFloor *record, double notional, double strike_rate, double observed_rate, double year_fraction, int32_t right)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(notional >= 0.0 && year_fraction >= 0.0 && (right == 1 || right == -1))) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->notional = notional;
    record->strike_rate = strike_rate;
    record->observed_rate = observed_rate;
    record->year_fraction = year_fraction;
    record->right = right;
    return UMI_STATUS_OK;
}

/* Return undiscounted caplet or floorlet payoff. */
double umi_quant_cap_floor_payoff(const UmiQuantCapFloor *record)
{
    if (record == NULL) return 0.0;
    return record->notional * record->year_fraction * fmax(((double)record->right) * (record->observed_rate - record->strike_rate), 0.0);
}
