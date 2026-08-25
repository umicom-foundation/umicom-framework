/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/interest_rate_swap.h
 *
 * PURPOSE:
 *   Represent fixed-versus-floating swap economics.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_INTEREST_RATE_SWAP_H
#define INCLUDE_UMICOM_FINANCE_QUANT_INTEREST_RATE_SWAP_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantInterestRateSwap {
    double notional;
    double fixed_rate;
    double floating_rate;
    double year_fraction;
} UmiQuantInterestRateSwap;

/* Initialise and validate the interest rate swap contract. */
UmiStatus umi_quant_interest_rate_swap_init(UmiQuantInterestRateSwap *record, double notional, double fixed_rate, double floating_rate, double year_fraction);

/* Return receive-floating/pay-fixed period value. */
double umi_quant_interest_rate_swap_period_value(const UmiQuantInterestRateSwap *record);

#ifdef __cplusplus
}
#endif

#endif
