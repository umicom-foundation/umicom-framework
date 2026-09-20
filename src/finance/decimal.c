/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/decimal.c
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

/*
 * The implementation is deliberately small and deterministic so callers can test decimal behaviour without starting a complete product.
 */

#include "umicom/finance/decimal.h"
/* Provide the decimal rescale operation used by this module and its client applications. */
/*
 * The original rescale body is retained below for migration reference.
 * Its unchecked multiplication could overflow a signed coefficient. The
 * replacement keeps the public name and truncation-towards-zero policy,
 * validates both scales, and checks every multiplication before evaluating it.
 */
// UmiStatus umi_decimal_rescale(UmiDecimal value,uint8_t target_scale,UmiDecimal *out){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out==NULL||target_scale>9U)return UMI_STATUS_INVALID_ARGUMENT;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(value.scale<target_scale){value.coefficient*=10;value.scale++;}/* Continue only while work remains available; the loop body advances the state on each pass. */ while(value.scale>target_scale){value.coefficient/=10;value.scale--;}*out=value;return UMI_STATUS_OK;}

#include <limits.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

UmiStatus umi_decimal_rescale(UmiDecimal value, uint8_t target_scale,
                              UmiDecimal *out)
{
    if (out == NULL || value.scale > 9U || target_scale > 9U)
        return UMI_STATUS_INVALID_ARGUMENT;
    while (value.scale < target_scale) {
        if (value.coefficient > INT64_MAX / 10 ||
            value.coefficient < INT64_MIN / 10)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value.coefficient *= 10;
        ++value.scale;
    }
    while (value.scale > target_scale) {
        value.coefficient /= 10;
        --value.scale;
    }
    *out = value;
    return UMI_STATUS_OK;
}

/* Parse fixed decimal text without strtod, locale or floating-point rounding.
 * A negative accumulator is unnecessary: uint64_t also represents abs(INT64_MIN).
 * Checked accumulation is performed against the limit for the requested sign. */
UmiStatus UmiDecimalParse(const char *text, size_t length, uint8_t scale,
                         UmiDecimal *out)
{
    if (text == NULL || out == NULL || scale > 9U || length == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_DECIMAL_TEXT_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t pos = 0U, end = length;
    while (pos < end && (text[pos] == ' ' || text[pos] == '\t')) ++pos;
    while (end > pos && (text[end - 1U] == ' ' || text[end - 1U] == '\t')) --end;
    int negative = 0, point = 0;
    if (pos < end && (text[pos] == '-' || text[pos] == '+')) {
        negative = text[pos] == '-';
        ++pos;
    }
    uint64_t limit = (uint64_t)INT64_MAX + (negative ? UINT64_C(1) : UINT64_C(0));
    uint64_t value = 0U;
    size_t wholeDigits = 0U, fractionDigits = 0U;
    for (; pos < end; ++pos) {
        unsigned char c = (unsigned char)text[pos];
        if (c == '.' && !point && wholeDigits != 0U && scale != 0U) {
            point = 1;
            continue;
        }
        if (c < '0' || c > '9') return UMI_STATUS_INVALID_ARGUMENT;
        if (point && fractionDigits >= scale) return UMI_STATUS_INVALID_ARGUMENT;
        uint64_t digit = (uint64_t)(c - '0');
        if (value > (limit - digit) / UINT64_C(10))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * UINT64_C(10) + digit;
        if (point) ++fractionDigits; else ++wholeDigits;
    }
    if (wholeDigits == 0U || (point && fractionDigits == 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    while (fractionDigits < scale) {
        if (value > limit / UINT64_C(10)) return UMI_STATUS_CAPACITY_EXCEEDED;
        value *= UINT64_C(10);
        ++fractionDigits;
    }
    UmiDecimal result;
    result.scale = scale;
    result.coefficient = negative
        ? (value == (uint64_t)INT64_MAX + UINT64_C(1)
            ? INT64_MIN : -(int64_t)value)
        : (int64_t)value;
    *out = result;
    return UMI_STATUS_OK;
}

/* Format into local storage first, so insufficient caller capacity never
 * exposes a truncated amount that a reader could mistake for another value. */
UmiStatus UmiDecimalFormat(UmiDecimal value, char *out, size_t capacity)
{
    if (out == NULL || value.scale > 9U) return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t magnitude = value.coefficient < 0
        ? (uint64_t)(-(value.coefficient + 1)) + UINT64_C(1)
        : (uint64_t)value.coefficient;
    uint64_t divisor = 1U;
    for (uint8_t index = 0U; index < value.scale; ++index) divisor *= UINT64_C(10);
    char temporary[UMI_DECIMAL_TEXT_MAX + 1U];
    int count;
    if (value.scale == 0U)
        count = snprintf(temporary, sizeof temporary, "%s%" PRIu64,
            value.coefficient < 0 ? "-" : "", magnitude);
    else
        count = snprintf(temporary, sizeof temporary, "%s%" PRIu64 ".%0*" PRIu64,
            value.coefficient < 0 ? "-" : "", magnitude / divisor,
            (int)value.scale, magnitude % divisor);
    if (count < 0) return UMI_STATUS_IO_ERROR;
    if ((size_t)count >= sizeof temporary || (size_t)count >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, temporary, (size_t)count + 1U);
    return UMI_STATUS_OK;
}
