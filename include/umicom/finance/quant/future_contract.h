/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/future_contract.h
 *
 * PURPOSE:
 *   Represent futures mark-to-market economics.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FUTURE_CONTRACT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FUTURE_CONTRACT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant future contract data shared with callers of this public contract.
 */
typedef struct UmiQuantFutureContract {
    double quantity;
    double multiplier;
    double entry_price;
    double mark_price;
} UmiQuantFutureContract;

/* Initialise and validate the future contract contract. */
UmiStatus umi_quant_future_contract_init(UmiQuantFutureContract *record, double quantity, double multiplier, double entry_price, double mark_price);

/* Return long-side futures mark-to-market value. */
double umi_quant_future_contract_mark_to_market(const UmiQuantFutureContract *record);

#ifdef __cplusplus
}
#endif

#endif
