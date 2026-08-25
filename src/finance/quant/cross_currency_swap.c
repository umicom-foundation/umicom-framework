/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/cross_currency_swap.c
 *
 * PURPOSE:
 *   Represent cross-currency principal exchange economics.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/cross_currency_swap.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_cross_currency_swap_init(UmiQuantCrossCurrencySwap *record, double domestic_principal, double foreign_principal, double fx_rate)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(domestic_principal >= 0.0 && foreign_principal >= 0.0 && fx_rate > 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->domestic_principal = domestic_principal;
    record->foreign_principal = foreign_principal;
    record->fx_rate = fx_rate;
    return UMI_STATUS_OK;
}

/* Return domestic principal minus converted foreign principal. */
double umi_quant_cross_currency_swap_principal_mismatch(const UmiQuantCrossCurrencySwap *record)
{
    if (record == NULL) return 0.0;
    return record->domestic_principal - record->foreign_principal * record->fx_rate;
}
