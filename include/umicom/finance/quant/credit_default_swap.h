/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/credit_default_swap.h
 *
 * PURPOSE:
 *   Represent simplified CDS expected-loss economics.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_CREDIT_DEFAULT_SWAP_H
#define INCLUDE_UMICOM_FINANCE_QUANT_CREDIT_DEFAULT_SWAP_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantCreditDefaultSwap {
    double notional;
    double default_probability;
    double recovery_rate;
} UmiQuantCreditDefaultSwap;

/* Initialise and validate the credit default swap contract. */
UmiStatus umi_quant_credit_default_swap_init(UmiQuantCreditDefaultSwap *record, double notional, double default_probability, double recovery_rate);

/* Return expected loss from default probability and recovery. */
double umi_quant_credit_default_swap_expected_loss(const UmiQuantCreditDefaultSwap *record);

#ifdef __cplusplus
}
#endif

#endif
