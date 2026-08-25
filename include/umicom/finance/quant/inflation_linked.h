/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/inflation_linked.h
 *
 * PURPOSE:
 *   Represent inflation-indexed principal adjustment.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_INFLATION_LINKED_H
#define INCLUDE_UMICOM_FINANCE_QUANT_INFLATION_LINKED_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiQuantInflationLinked {
    double base_index;
    double current_index;
    double principal;
} UmiQuantInflationLinked;

/* Initialise and validate the inflation linked contract. */
UmiStatus umi_quant_inflation_linked_init(UmiQuantInflationLinked *record, double base_index, double current_index, double principal);

/* Return principal scaled by the current/base inflation index ratio. */
double umi_quant_inflation_linked_adjusted_principal(const UmiQuantInflationLinked *record);

#ifdef __cplusplus
}
#endif

#endif
