/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/valuation_adjustment.h
 *
 * PURPOSE:
 *   Represent one discounted valuation-adjustment component.
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

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_VALUATION_ADJUSTMENT_H
#define INCLUDE_UMICOM_FINANCE_QUANT_VALUATION_ADJUSTMENT_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantValuationAdjustment {
    double exposure;
    double probability;
    double loss_given_event;
    double discount_factor;
} UmiQuantValuationAdjustment;

/* Initialise and validate the valuation adjustment contract. */
UmiStatus umi_quant_valuation_adjustment_init(UmiQuantValuationAdjustment *record, double exposure, double probability, double loss_given_event, double discount_factor);

/* Return discounted expected valuation adjustment. */
double umi_quant_valuation_adjustment_amount(const UmiQuantValuationAdjustment *record);

#ifdef __cplusplus
}
#endif

#endif
