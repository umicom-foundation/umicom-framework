/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/forward_contract.h
 *
 * PURPOSE:
 *   Represent linear forward payoff economics.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_FORWARD_CONTRACT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_FORWARD_CONTRACT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantForwardContract {
    double quantity;
    double strike;
    double spot;
} UmiQuantForwardContract;

/* Initialise and validate the forward contract contract. */
UmiStatus umi_quant_forward_contract_init(UmiQuantForwardContract *record, double quantity, double strike, double spot);

/* Return long-forward intrinsic payoff before discounting. */
double umi_quant_forward_contract_payoff(const UmiQuantForwardContract *record);

#ifdef __cplusplus
}
#endif

#endif
