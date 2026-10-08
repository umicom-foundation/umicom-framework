/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/decimal_scientific.c
 * PURPOSE: Parse provider decimal notation exactly, without locale or floating-point rounding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/decimal.h"
#include <stdint.h>

UmiStatus UmiDecimalParseScientificExact(const char *text, size_t length, UmiDecimal *out)
{
    if (text == NULL || out == NULL || length == 0U || length > UMI_DECIMAL_TEXT_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t at = 0U, count = 0U;
    char digits[UMI_DECIMAL_TEXT_MAX];
    bool negative = false;
    if (text[at] == '+' || text[at] == '-')
    {
        negative = text[at] == '-';
        ++at;
    }
    size_t whole = at;
    while (at < length && text[at] >= '0' && text[at] <= '9')
        digits[count++] = text[at++];
    if (at == whole)
        return UMI_STATUS_PARSE_ERROR;
    int fraction = 0;
    if (at < length && text[at] == '.')
    {
        ++at;
        size_t start = at;
        while (at < length && text[at] >= '0' && text[at] <= '9')
        {
            digits[count++] = text[at++];
            ++fraction;
        }
        if (at == start)
            return UMI_STATUS_PARSE_ERROR;
    }
    int exponent = 0;
    if (at < length && (text[at] == 'e' || text[at] == 'E'))
    {
        ++at;
        bool descending = false;
        if (at < length && (text[at] == '+' || text[at] == '-'))
        {
            descending = text[at] == '-';
            ++at;
        }
        size_t start = at;
        while (at < length && text[at] >= '0' && text[at] <= '9')
        {
            unsigned digit = (unsigned)(text[at++] - '0');
            if (exponent > 100 || (exponent == 100 && digit > 0U))
                return UMI_STATUS_CAPACITY_EXCEEDED;
            exponent = exponent * 10 + (int)digit;
        }
        if (at == start)
            return UMI_STATUS_PARSE_ERROR;
        if (descending)
            exponent = -exponent;
    }
    if (at != length)
        return UMI_STATUS_PARSE_ERROR;
    size_t first = 0U;
    while (first < count && digits[first] == '0')
        ++first;
    UmiDecimal value = {0};
    if (first == count)
    {
        *out = value;
        return UMI_STATUS_OK;
    }
    int scale = fraction - exponent;
    /* Removing trailing zero digits changes representation only. Nonzero digits
     * outside the supported precision remain an error, never an approximation. */
    while (count > first && digits[count - 1U] == '0' && scale > 0)
    {
        --count;
        --scale;
    }
    if (scale > 9 || (scale < 0 && (size_t)(-scale) > 19U) ||
        count - first + (scale < 0 ? (size_t)(-scale) : 0U) > 19U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t magnitude = 0U;
    const uint64_t maximum = negative ? (uint64_t)INT64_MAX + 1U : (uint64_t)INT64_MAX;
    for (size_t index = first; index < count; ++index)
    {
        unsigned digit = (unsigned)(digits[index] - '0');
        if (magnitude > (maximum - digit) / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        magnitude = magnitude * 10U + digit;
    }
    while (scale < 0)
    {
        if (magnitude > maximum / 10U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        magnitude *= 10U;
        ++scale;
    }
    value.coefficient = negative ? (magnitude == (uint64_t)INT64_MAX + 1U ? INT64_MIN : -(int64_t)magnitude)
                                 : (int64_t)magnitude;
    value.scale = (uint8_t)scale;
    *out = value;
    return UMI_STATUS_OK;
}
