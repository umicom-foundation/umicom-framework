/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/money_text.h
 *
 * PURPOSE:
 *   Format canonical money without floating-point conversion or locale dependence.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_MONEY_TEXT_H
#define UMICOM_FINANCE_MONEY_TEXT_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Enough for a three-letter code, space, sign, zero, point, 255 decimal
 * places and a terminator. Scale is a uint8_t in the canonical money record. */
#define UMI_MONEY_TEXT_CAPACITY 264U
/** Format e.g. GBP -0.01. All scales 0..255 and INT64_MIN are supported.
 * The currency must contain exactly three ASCII capital letters and a NUL.
 * requiredCapacity, when non-NULL, receives the size INCLUDING the terminator.
 * NULL output with zero capacity is a successful size query. Other failures
 * leave output unchanged, never a truncated financial value. This function
 * does not change, rescale, round or validate the economics of the amount. */
UmiStatus UmiMoneyTextFormat(const UmiMoney *money, char *output,
    size_t capacity, size_t *requiredCapacity);
#ifdef __cplusplus
}
#endif
#endif
