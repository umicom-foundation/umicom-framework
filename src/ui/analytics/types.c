/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/types.c
 *
 * PURPOSE:
 *   Define stable toolkit-neutral analytics limits, enums and utility operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/types.h"

#include <math.h>
#include <string.h>
/*
 * Provide the analytics copy text operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
/* Check that analytics number satisfies its contract before another service relies on it. */
int umi_analytics_number_valid(double value) { return isfinite(value) ? 1 : 0; }
/* Provide the analytics compare operation used by this module and its client applications. */
int umi_analytics_compare(double left, UmiAnalyticsCompare op, double right) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (!isfinite(left) || !isfinite(right)) return 0;
    /* Select the behaviour associated with the requested command or state value. */
    switch (op) {
        case UMI_ANALYTICS_COMPARE_GT: return left > right;
        case UMI_ANALYTICS_COMPARE_GTE: return left >= right;
        case UMI_ANALYTICS_COMPARE_LT: return left < right;
        case UMI_ANALYTICS_COMPARE_LTE: return left <= right;
        case UMI_ANALYTICS_COMPARE_EQ: return left == right;
        case UMI_ANALYTICS_COMPARE_NE: return left != right;
        default: return 0;
    }
}
/*
 * Provide the analytics hash text operation used by this module and its client
 * applications.
 */
uint64_t umi_analytics_hash_text(const char *text) {
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor = (const unsigned char *)text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != 0U) { hash ^= (uint64_t)(*cursor++); hash *= UINT64_C(1099511628211); }
    return hash;
}
