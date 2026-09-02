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

/*
 * Provide the treasury bounded length operation used by this module and its client
 * applications.
 */
static size_t umi_treasury_bounded_length(const char *value, size_t capacity) {
    size_t length = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (length < capacity && value[length] != '\0') ++length;
    return length;
}
/* Check that treasury id satisfies its contract before another service relies on it. */
bool umi_treasury_id_valid(const char *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value[0] == '\0') return false;
    return umi_treasury_bounded_length(value, (size_t)UMI_TREASURY_ID_CAPACITY) < (size_t)UMI_TREASURY_ID_CAPACITY;
}
/*
 * Copy treasury id into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_treasury_id_copy(char *destination, size_t capacity, const char *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = umi_treasury_bounded_length(value, capacity);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, value, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}
/*
 * Provide the treasury currency from code operation used by this module and its client
 * applications.
 */
UmiStatus umi_treasury_currency_from_code(const char *code, UmiCurrency *out_currency) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (code == NULL || out_currency == NULL || strlen(code) != 3U) return UMI_STATUS_INVALID_ARGUMENT;
    out_currency->code[0]=code[0]; out_currency->code[1]=code[1]; out_currency->code[2]=code[2]; out_currency->code[3]='\0';
    return UMI_STATUS_OK;
}
/*
 * Provide the treasury currency equal operation used by this module and its client
 * applications.
 */
bool umi_treasury_currency_equal(UmiCurrency left, UmiCurrency right) { return memcmp(left.code, right.code, 4U)==0; }
/* Provide the treasury abs i64 operation used by this module and its client applications. */
int64_t umi_treasury_abs_i64(int64_t value) { /* Apply this branch only when its contract condition is satisfied. */ if (value == INT64_MIN) return INT64_MAX; return value < 0 ? -value : value; }
