/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/smile_slice.h
 *
 * PURPOSE:
 *   Represent simple strike-volatility smile slope evidence.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_SMILE_SLICE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_SMILE_SLICE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant smile slice data shared with callers of this public contract.
 */
typedef struct UmiQuantSmileSlice {
    double lower_strike;
    double lower_vol;
    double upper_strike;
    double upper_vol;
} UmiQuantSmileSlice;

/* Initialise and validate the smile slice contract. */
UmiStatus umi_quant_smile_slice_init(UmiQuantSmileSlice *record, double lower_strike, double lower_vol, double upper_strike, double upper_vol);

/* Return linear volatility smile slope between two strikes. */
double umi_quant_smile_slice_slope(const UmiQuantSmileSlice *record);

#ifdef __cplusplus
}
#endif

#endif
