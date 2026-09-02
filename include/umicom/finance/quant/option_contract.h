/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/option_contract.h
 *
 * PURPOSE:
 *   Represent vanilla option intrinsic economics.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_OPTION_CONTRACT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_OPTION_CONTRACT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant option contract data shared with callers of this public contract.
 */
typedef struct UmiQuantOptionContract {
    double quantity;
    double strike;
    double underlying;
    int32_t right;
} UmiQuantOptionContract;

/* Initialise and validate the option contract contract. */
UmiStatus umi_quant_option_contract_init(UmiQuantOptionContract *record, double quantity, double strike, double underlying, int32_t right);

/* Return call or put intrinsic value for the configured quantity. */
double umi_quant_option_contract_intrinsic(const UmiQuantOptionContract *record);

#ifdef __cplusplus
}
#endif

#endif
