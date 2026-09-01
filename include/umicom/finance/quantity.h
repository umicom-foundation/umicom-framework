/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quantity.h
 *
 * PURPOSE:
 *   Validate finite positive trading quantities without owning instrument semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module keeps quantity logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_QUANTITY_H
#define INCLUDE_UMICOM_FINANCE_QUANTITY_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
int umi_quantity_positive(UmiQuantity quantity);
UmiQuantity umi_quantity_signed(UmiQuantity quantity, int direction);
#ifdef __cplusplus
}
#endif
#endif
