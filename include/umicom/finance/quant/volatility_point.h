/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/volatility_point.h
 *
 * PURPOSE:
 *   Represent one volatility observation by expiry and strike.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_VOLATILITY_POINT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_VOLATILITY_POINT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantVolatilityPoint {
    double expiry_years;
    double strike;
    double volatility;
} UmiQuantVolatilityPoint;

/* Initialise and validate the volatility point contract. */
UmiStatus umi_quant_volatility_point_init(UmiQuantVolatilityPoint *record, double expiry_years, double strike, double volatility);

/* Return total variance volatility squared times expiry. */
double umi_quant_volatility_point_variance(const UmiQuantVolatilityPoint *record);

#ifdef __cplusplus
}
#endif

#endif
