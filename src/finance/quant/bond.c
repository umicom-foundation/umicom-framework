/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/quant/bond.c
 *
 * PURPOSE:
 *   Represent fixed-rate bond coupon economics.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/quant/bond.h"

#include <math.h>
#include <string.h>

/* Validate financial inputs before making the record observable to callers. */
UmiStatus umi_quant_bond_init(UmiQuantBond *record, double face_value, double coupon_rate, int32_t payments_per_year, int32_t maturity_days)
{
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!(face_value > 0.0 && coupon_rate >= 0.0 && payments_per_year > 0 && maturity_days > 0)) return UMI_STATUS_INVALID_ARGUMENT;
    memset(record, 0, sizeof *record);
    record->face_value = face_value;
    record->coupon_rate = coupon_rate;
    record->payments_per_year = payments_per_year;
    record->maturity_days = maturity_days;
    return UMI_STATUS_OK;
}

/* Return one scheduled fixed coupon payment. */
double umi_quant_bond_coupon_payment(const UmiQuantBond *record)
{
    if (record == NULL) return 0.0;
    return record->face_value * record->coupon_rate / (double)record->payments_per_year;
}
