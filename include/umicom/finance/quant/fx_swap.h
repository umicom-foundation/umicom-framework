/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/fx_swap.h
 *
 * PURPOSE:
 *   Model an FX swap using near and far forward rates.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FX_SWAP_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FX_SWAP_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantFxSwap {
    double near_rate;
    double far_rate;
    int32_t far_days;
} UmiQuantFxSwap;

/* Initialise and validate the fx swap contract. */
UmiStatus umi_quant_fx_swap_init(UmiQuantFxSwap *record, double near_rate, double far_rate, int32_t far_days);

/* Return the far-minus-near swap points. */
double umi_quant_fx_swap_swap_points(const UmiQuantFxSwap *record);

#ifdef __cplusplus
}
#endif

#endif
