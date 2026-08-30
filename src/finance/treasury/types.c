/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/types.c
 *
 * PURPOSE:
 *   Implement common validation and bounded-copy helpers for the treasury financial platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/types.h"
#include <limits.h>
#include <string.h>

static size_t umi_treasury_bounded_length(const char *value, size_t capacity) {
    size_t length = 0U;
    if (value == NULL) return 0U;
    while (length < capacity && value[length] != '\0') ++length;
    return length;
}
bool umi_treasury_id_valid(const char *value) {
    if (value == NULL || value[0] == '\0') return false;
    return umi_treasury_bounded_length(value, (size_t)UMI_TREASURY_ID_CAPACITY) < (size_t)UMI_TREASURY_ID_CAPACITY;
}
UmiStatus umi_treasury_id_copy(char *destination, size_t capacity, const char *value) {
    if (destination == NULL || capacity == 0U || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = umi_treasury_bounded_length(value, capacity);
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, value, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
UmiStatus umi_treasury_currency_from_code(const char *code, UmiCurrency *out_currency) {
    if (code == NULL || out_currency == NULL || strlen(code) != 3U) return UMI_STATUS_INVALID_ARGUMENT;
    out_currency->code[0]=code[0]; out_currency->code[1]=code[1]; out_currency->code[2]=code[2]; out_currency->code[3]='\0';
    return UMI_STATUS_OK;
}
bool umi_treasury_currency_equal(UmiCurrency left, UmiCurrency right) { return memcmp(left.code, right.code, 4U)==0; }
int64_t umi_treasury_abs_i64(int64_t value) { if (value == INT64_MIN) return INT64_MAX; return value < 0 ? -value : value; }
