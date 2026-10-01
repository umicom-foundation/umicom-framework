/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/money_rate.c
 * PURPOSE: Centralise portable exact percentage arithmetic and explicit rounding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/money_rate.h"
#include "umicom/finance/currency.h"
#include <stdbool.h>

UmiStatus UmiMinorApplyBasisPoints(int64_t amount, uint32_t basisPoints,
    UmiMoneyRounding rounding, int64_t *out)
{
    if (out == NULL || basisPoints > 10000U || rounding < UMI_MONEY_TOWARD_ZERO ||
        rounding > UMI_MONEY_AWAY_FROM_ZERO) return UMI_STATUS_INVALID_ARGUMENT;
    /* Splitting the magnitude bounds the first product by 2^63 and the second
     * below 10000^2. Even rounding up cannot exceed the input magnitude because
     * the rate is at most 100%. abs(INT64_MIN) is representable as uint64_t. */
    uint64_t magnitude = amount < 0 ? (uint64_t)(-(amount + 1)) + UINT64_C(1) : (uint64_t)amount;
    uint64_t small = (magnitude % UINT64_C(10000)) * basisPoints;
    uint64_t whole = (magnitude / UINT64_C(10000)) * basisPoints + small / UINT64_C(10000);
    uint64_t remainder = small % UINT64_C(10000);
    bool increment = (rounding == UMI_MONEY_AWAY_FROM_ZERO && remainder != 0U) ||
        (rounding == UMI_MONEY_HALF_AWAY && remainder >= 5000U) ||
        (rounding == UMI_MONEY_HALF_EVEN && (remainder > 5000U || (remainder == 5000U && (whole & 1U) != 0U)));
    if (increment) ++whole;
    *out = amount < 0 ? (whole == (uint64_t)INT64_MAX + UINT64_C(1) ? INT64_MIN : -(int64_t)whole) : (int64_t)whole;
    return UMI_STATUS_OK;
}
UmiStatus UmiMoneyApplyBasisPoints(const UmiMoney *amount, uint32_t basisPoints,
    UmiMoneyRounding rounding, UmiMoney *out)
{
    if (amount == NULL || out == NULL || !umi_currency_valid(&amount->currency) || amount->scale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiMoney result = *amount;
    UmiStatus status = UmiMinorApplyBasisPoints(amount->minor_units, basisPoints, rounding, &result.minor_units);
    if (status == UMI_STATUS_OK) *out = result;
    return status;
}
const char *UmiMoneyRoundingName(UmiMoneyRounding rounding)
{
    switch (rounding) {
    case UMI_MONEY_TOWARD_ZERO: return "toward zero";
    case UMI_MONEY_HALF_AWAY: return "nearest, ties away from zero";
    case UMI_MONEY_HALF_EVEN: return "nearest, ties to even";
    case UMI_MONEY_AWAY_FROM_ZERO: return "away from zero";
    default: return "invalid rounding policy";
    }
}
