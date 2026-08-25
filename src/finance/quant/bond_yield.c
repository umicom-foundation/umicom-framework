/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/bond_yield.c
 *
 * PURPOSE:
 *   Capture bond price and annual coupon for yield approximations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/bond_yield.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_bond_yield_init(UmiQuantBondYield *record, double price, double face_value, double annual_coupon, double years_to_maturity)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(price > 0.0 && face_value > 0.0 && years_to_maturity > 0.0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->price = price;
    record->face_value = face_value;
    record->annual_coupon = annual_coupon;
    record->years_to_maturity = years_to_maturity;
    return UMI_STATUS_OK;
}

/* Estimate yield using coupon plus pull-to-par over average capital. */
double umi_quant_bond_yield_approximate_yield(const UmiQuantBondYield *record)
{
    if (record == NULL) return 0.0;
    return (record->annual_coupon + (record->face_value - record->price) / record->years_to_maturity) / ((record->face_value + record->price) * 0.5);
}
