/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/decimal.h
 *
 * PURPOSE:
 *   Represent fixed-scale decimal coefficients for cash and control calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module keeps decimal logic in one reusable Framework boundary so financial applications do not copy the same rules.
 */

#ifndef INCLUDE_UMICOM_FINANCE_DECIMAL_H
#define INCLUDE_UMICOM_FINANCE_DECIMAL_H
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_decimal_rescale(UmiDecimal value, uint8_t target_scale, UmiDecimal *out);
#ifdef __cplusplus
}
#endif
#endif
