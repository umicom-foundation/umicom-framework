/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/bond_yield.h
 *
 * PURPOSE:
 *   Capture bond price and annual coupon for yield approximations.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BOND_YIELD_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BOND_YIELD_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantBondYield {
    double price;
    double face_value;
    double annual_coupon;
    double years_to_maturity;
} UmiQuantBondYield;

/* Initialise and validate the bond yield contract. */
UmiStatus umi_quant_bond_yield_init(UmiQuantBondYield *record, double price, double face_value, double annual_coupon, double years_to_maturity);

/* Estimate yield using coupon plus pull-to-par over average capital. */
double umi_quant_bond_yield_approximate_yield(const UmiQuantBondYield *record);

#ifdef __cplusplus
}
#endif

#endif
