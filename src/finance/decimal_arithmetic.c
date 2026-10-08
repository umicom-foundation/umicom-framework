/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/decimal_arithmetic.c
 * PURPOSE: Compare and combine fixed-point values without rounding or signed overflow.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/decimal.h"
#include <stdint.h>

static uint64_t DecimalMagnitude(int64_t coefficient)
{
    return coefficient < 0 ? (uint64_t)(-(coefficient + 1)) + UINT64_C(1) : (uint64_t)coefficient;
}

static uint64_t DecimalDivisor(uint8_t places)
{
    uint64_t divisor = 1U;
    while (places != 0U)
    {
        divisor *= 10U;
        --places;
    }
    return divisor;
}

UmiStatus UmiDecimalCompare(UmiDecimal left, UmiDecimal right, int *outComparison)
{
    if (outComparison == NULL || left.scale > 9U || right.scale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;
    int comparison;
    if ((left.coefficient < 0) != (right.coefficient < 0))
        comparison = left.coefficient < 0 ? -1 : 1;
    else
    {
        const uint64_t a = DecimalMagnitude(left.coefficient);
        const uint64_t b = DecimalMagnitude(right.coefficient);
        if (left.scale == right.scale)
            comparison = (a > b) - (a < b);
        else if (left.scale > right.scale)
        {
            const uint64_t divisor = DecimalDivisor((uint8_t)(left.scale - right.scale));
            const uint64_t whole = a / divisor;
            comparison = whole == b ? (a % divisor != 0U ? 1 : 0) : (whole > b ? 1 : -1);
        }
        else
        {
            const uint64_t divisor = DecimalDivisor((uint8_t)(right.scale - left.scale));
            const uint64_t whole = b / divisor;
            comparison = a == whole ? (b % divisor != 0U ? -1 : 0) : (a > whole ? 1 : -1);
        }
        if (left.coefficient < 0)
            comparison = -comparison;
    }
    *outComparison = comparison;
    return UMI_STATUS_OK;
}

UmiStatus UmiDecimalAddExact(UmiDecimal left, UmiDecimal right, UmiDecimal *out)
{
    if (out == NULL || left.scale > 9U || right.scale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;
    const uint8_t scale = left.scale > right.scale ? left.scale : right.scale;
    UmiDecimal a, b;
    UmiStatus status = umi_decimal_rescale(left, scale, &a);
    if (status != UMI_STATUS_OK)
        return status;
    status = umi_decimal_rescale(right, scale, &b);
    if (status != UMI_STATUS_OK)
        return status;
    /* Retain the finer input scale. Refuse coefficients that cannot fit at that
     * scale instead of silently removing fractional digits from an order. */
    if ((b.coefficient > 0 && a.coefficient > INT64_MAX - b.coefficient) ||
        (b.coefficient < 0 && a.coefficient < INT64_MIN - b.coefficient))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDecimal sum = {0};
    sum.coefficient = a.coefficient + b.coefficient;
    sum.scale = scale;
    *out = sum;
    return UMI_STATUS_OK;
}

UmiStatus UmiDecimalIsMultiple(UmiDecimal value, UmiDecimal increment, bool *out)
{
    if (out == NULL || value.scale > 9U || increment.scale > 9U || increment.coefficient <= 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t magnitude = DecimalMagnitude(value.coefficient);
    uint64_t divisor = (uint64_t)increment.coefficient;
    if (value.scale > increment.scale)
    {
        uint64_t fraction = DecimalDivisor((uint8_t)(value.scale - increment.scale));
        if (magnitude % fraction != 0U)
        {
            *out = false;
            return UMI_STATUS_OK;
        }
        magnitude /= fraction;
    }
    uint64_t remainder = magnitude % divisor;
    /* Multiply the remainder, not the full coefficient. Each addition is below
     * twice INT64_MAX, so even the widest valid decimal remains representable. */
    for (uint8_t scale = value.scale; scale < increment.scale; ++scale)
    {
        uint64_t next = 0U;
        for (unsigned digit = 0U; digit < 10U; ++digit)
            next = (next + remainder) % divisor;
        remainder = next;
    }
    *out = remainder == 0U;
    return UMI_STATUS_OK;
}
