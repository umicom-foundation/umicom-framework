/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/bond_pricer.h
 *
 * PURPOSE:
 *   Price fixed-rate bonds from regular coupons and a flat yield.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_BOND_PRICER_H
#define INCLUDE_UMICOM_FINANCE_QUANT_BOND_PRICER_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Price a regular fixed-coupon bond using per-period compound discounting. */
UmiStatus umi_quant_bond_pricer_price(double face_value, double coupon_rate, int32_t payments_per_year, int32_t periods, double yield_rate, double *out_price);

#ifdef __cplusplus
}
#endif

#endif
