/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/types.c
 *
 * PURPOSE:
 *   Define stable toolkit-neutral analytics limits, enums and utility operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/types.h"

#include <math.h>
#include <string.h>
UmiStatus umi_analytics_copy_text(char *destination, size_t capacity, const char *source) {
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}
int umi_analytics_number_valid(double value) { return isfinite(value) ? 1 : 0; }
int umi_analytics_compare(double left, UmiAnalyticsCompare op, double right) {
    if (!isfinite(left) || !isfinite(right)) return 0;
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
uint64_t umi_analytics_hash_text(const char *text) {
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *cursor = (const unsigned char *)text;
    if (text == NULL) return 0U;
    while (*cursor != 0U) { hash ^= (uint64_t)(*cursor++); hash *= UINT64_C(1099511628211); }
    return hash;
}
