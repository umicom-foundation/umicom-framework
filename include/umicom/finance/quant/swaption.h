/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/swaption.h
 *
 * PURPOSE:
 *   Represent a vanilla option on a swap rate.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SWAPTION_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SWAPTION_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant swaption data shared with callers of this public contract.
 */
typedef struct UmiQuantSwaption {
    double annuity;
    double strike_rate;
    double forward_swap_rate;
    int32_t right;
} UmiQuantSwaption;

/* Initialise and validate the swaption contract. */
UmiStatus umi_quant_swaption_init(UmiQuantSwaption *record, double annuity, double strike_rate, double forward_swap_rate, int32_t right);

/* Return annuity-scaled intrinsic swaption value. */
double umi_quant_swaption_intrinsic(const UmiQuantSwaption *record);

#ifdef __cplusplus
}
#endif

#endif
