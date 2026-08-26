/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/types.c
 *
 * PURPOSE:
 *   Define common prudential capital, liquidity and enterprise stress-testing primitives.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/types.h"

#include <math.h>
#include <string.h>

UmiStatus umi_pru_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

uint64_t umi_pru_hash_text(const char *text)
{
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor;
    if (text == NULL) return 0U;
    cursor = (const unsigned char *)text;
    while (*cursor != 0U) {
        hash ^= (uint64_t)*cursor;
        hash *= UINT64_C(1099511628211);
        ++cursor;
    }
    return hash;
}

int umi_pru_number_valid(double value) { return isfinite(value) ? 1 : 0; }

UmiStatus umi_pru_ratio(double numerator, double denominator, double *out_ratio)
{
    if (out_ratio == NULL || !isfinite(numerator) || !isfinite(denominator) || denominator == 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_ratio = numerator / denominator;
    return isfinite(*out_ratio) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_pru_weighted_amount(double amount, double weight, double *out_amount)
{
    if (out_amount == NULL || !isfinite(amount) || !isfinite(weight) || amount < 0.0 || weight < 0.0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_amount = amount * weight;
    return isfinite(*out_amount) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
