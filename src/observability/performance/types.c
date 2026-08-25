/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/performance/types.c
 *
 * PURPOSE:
 *   Implement bounded utility logic shared by Framework profiling, telemetry, benchmark, coverage and dashboard state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/observability/performance/types.h"

/* Copy character-by-character so truncation is always reported to the caller. */
UmiStatus umi_performance_copy_text(char *destination, size_t capacity, const char *source) {
    size_t i = 0U;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    while (source[i] != '\0' && i + 1U < capacity) {
        destination[i] = source[i];
        ++i;
    }
    if (source[i] != '\0') {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    destination[i] = '\0';
    return UMI_STATUS_OK;
}

/* Accept only identifiers that are non-empty and terminate inside the public capacity. */
bool umi_performance_id_valid(const char *id) {
    size_t i = 0U;
    if (id == NULL || id[0] == '\0') {
        return false;
    }
    while (i < UMI_PERFORMANCE_ID_CAPACITY) {
        if (id[i] == '\0') {
            return true;
        }
        ++i;
    }
    return false;
}

/* Clamp normalized health/quality values before they are exposed to frontends. */
double umi_performance_clamp_unit(double value) {
    if (value < 0.0) return 0.0;
    if (value > 1.0) return 1.0;
    return value;
}

/* Express a regression/improvement as a percentage of the baseline. */
double umi_performance_percent_delta(double current, double baseline) {
    if (baseline == 0.0) return 0.0;
    return ((current - baseline) / baseline) * 100.0;
}

/* Prevent divide-by-zero from contaminating toolkit-neutral telemetry state. */
double umi_performance_safe_ratio(double numerator, double denominator) {
    if (denominator == 0.0) return 0.0;
    return numerator / denominator;
}

/* Use a deterministic 64-bit FNV-1a hash for lightweight evidence fingerprints. */
uint64_t umi_performance_hash_text(const char *text) {
    uint64_t hash = UINT64_C(1469598103934665603);
    size_t i = 0U;
    if (text == NULL) return 0U;
    while (text[i] != '\0') {
        hash ^= (uint64_t)(unsigned char)text[i];
        hash *= UINT64_C(1099511628211);
        ++i;
    }
    return hash;
}

/* Map a scalar measurement onto the common observability severity scale. */
UmiPerformanceSeverity umi_performance_severity_from_thresholds(double value, double warning_threshold, double critical_threshold) {
    if (value >= critical_threshold) return UMI_PERFORMANCE_SEVERITY_CRITICAL;
    if (value >= warning_threshold) return UMI_PERFORMANCE_SEVERITY_WARNING;
    return UMI_PERFORMANCE_SEVERITY_INFO;
}
