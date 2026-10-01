/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/money_rate.h
 * PURPOSE: Apply explicit basis-point rounding without floating-point money arithmetic.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_FINANCE_MONEY_RATE_H
#define UMICOM_FINANCE_MONEY_RATE_H
#include "umicom/finance/money.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Rounding applies to the magnitude; the original sign is restored afterwards.
 * HALF_AWAY rounds ties away from zero; HALF_EVEN rounds ties to an even integer.
 * No policy is silently selected for new callers. */
typedef enum UmiMoneyRounding {
    UMI_MONEY_TOWARD_ZERO = 0,
    UMI_MONEY_HALF_AWAY = 1,
    UMI_MONEY_HALF_EVEN = 2,
    UMI_MONEY_AWAY_FROM_ZERO = 3
} UmiMoneyRounding;
/** Calculate amount * basisPoints / 10000 for rates 0..10000 (0..100%).
 * Supports the whole int64_t range, including INT64_MIN. Never uses a wider
 * compiler-specific integer or floating point. Failure leaves output unchanged. */
UmiStatus UmiMinorApplyBasisPoints(int64_t amount, uint32_t basisPoints,
    UmiMoneyRounding rounding, int64_t *out);
/** Keep a valid currency and explicit scale (0..9). The output may alias input. */
UmiStatus UmiMoneyApplyBasisPoints(const UmiMoney *amount, uint32_t basisPoints,
    UmiMoneyRounding rounding, UmiMoney *out);
const char *UmiMoneyRoundingName(UmiMoneyRounding rounding);
#ifdef __cplusplus
}
#endif
#endif
