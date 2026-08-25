/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/fra.h
 *
 * PURPOSE:
 *   Represent a forward-rate agreement payoff contract.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FRA_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FRA_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantFra {
    double notional;
    double fixed_rate;
    double forward_rate;
    int32_t days;
    int32_t day_basis;
} UmiQuantFra;

/* Initialise and validate the fra contract. */
UmiStatus umi_quant_fra_init(UmiQuantFra *record, double notional, double fixed_rate, double forward_rate, int32_t days, int32_t day_basis);

/* Calculate the undiscounted FRA rate-difference payoff. */
double umi_quant_fra_undiscounted_payoff(const UmiQuantFra *record);

#ifdef __cplusplus
}
#endif

#endif
