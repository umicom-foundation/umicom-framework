/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/money.c
 *
 * PURPOSE:
 *   Perform safe same-currency money addition and subtraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The implementation is deliberately small and deterministic so callers can test money behaviour without starting a complete product.
 */

#include <stdint.h>
#include <string.h>
#include "umicom/finance/money.h"
#include "umicom/finance/currency.h"

/* Both operands must use the same currency and scale. Validate the fixed
 * currency fields before comparison, including their terminating zero byte. */
static int same_money_shape(const UmiMoney *a, const UmiMoney *b)
{
    return a != NULL && b != NULL && a->scale == b->scale &&
        umi_currency_valid(&a->currency) && umi_currency_valid(&b->currency) &&
        memcmp(a->currency.code, b->currency.code, sizeof(a->currency.code)) == 0;
}

/* Reject overflow before evaluation. A failed operation never alters output;
 * a temporary also permits out to alias either operand safely. */
UmiStatus umi_money_add(const UmiMoney *left, const UmiMoney *right, UmiMoney *out)
{
    UmiMoney result;
    if (out == NULL || !same_money_shape(left, right)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((right->minor_units > 0 && left->minor_units > INT64_MAX - right->minor_units) ||
        (right->minor_units < 0 && left->minor_units < INT64_MIN - right->minor_units))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    result = *left;
    result.minor_units = left->minor_units + right->minor_units;
    *out = result;
    return UMI_STATUS_OK;
}

/* Check subtraction directly: negating INT64_MIN would itself overflow. */
UmiStatus umi_money_subtract(const UmiMoney *left, const UmiMoney *right, UmiMoney *out)
{
    UmiMoney result;
    if (out == NULL || !same_money_shape(left, right)) return UMI_STATUS_INVALID_ARGUMENT;
    if ((right->minor_units > 0 && left->minor_units < INT64_MIN + right->minor_units) ||
        (right->minor_units < 0 && left->minor_units > INT64_MAX + right->minor_units))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    result = *left;
    result.minor_units = left->minor_units - right->minor_units;
    *out = result;
    return UMI_STATUS_OK;
}
