/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/bond.h
 *
 * PURPOSE:
 *   Represent fixed-rate bond coupon economics.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BOND_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BOND_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantBond {
    double face_value;
    double coupon_rate;
    int32_t payments_per_year;
    int32_t maturity_days;
} UmiQuantBond;

/* Initialise and validate the bond contract. */
UmiStatus umi_quant_bond_init(UmiQuantBond *record, double face_value, double coupon_rate, int32_t payments_per_year, int32_t maturity_days);

/* Return one scheduled fixed coupon payment. */
double umi_quant_bond_coupon_payment(const UmiQuantBond *record);

#ifdef __cplusplus
}
#endif

#endif
