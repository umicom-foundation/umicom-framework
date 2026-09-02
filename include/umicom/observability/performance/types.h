/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/types.h
 *
 * PURPOSE:
 *   Define stable C23 types and bounded utility functions shared by the Framework performance/observability engineering control plane.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_TYPES_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PERFORMANCE_ID_CAPACITY 96U
#define UMI_PERFORMANCE_TEXT_CAPACITY 256U
#define UMI_PERFORMANCE_API_VERSION 1U

/**
 * List the named performance state values accepted by this public contract.
 */
typedef enum UmiPerformanceState {
    UMI_PERFORMANCE_STATE_IDLE = 0,
    UMI_PERFORMANCE_STATE_ACTIVE = 1,
    UMI_PERFORMANCE_STATE_COMPLETE = 2,
    UMI_PERFORMANCE_STATE_FAILED = 3
} UmiPerformanceState;

/**
 * List the named performance severity values accepted by this public contract.
 */
typedef enum UmiPerformanceSeverity {
    UMI_PERFORMANCE_SEVERITY_INFO = 0,
    UMI_PERFORMANCE_SEVERITY_WARNING = 1,
    UMI_PERFORMANCE_SEVERITY_ERROR = 2,
    UMI_PERFORMANCE_SEVERITY_CRITICAL = 3
} UmiPerformanceSeverity;

/* Copy bounded text without accepting silent truncation. */
UmiStatus umi_performance_copy_text(char *destination, size_t capacity, const char *source);
/* Validate a stable non-empty identifier stored in a bounded buffer. */
bool umi_performance_id_valid(const char *id);
/* Clamp a normalized score into the inclusive 0..1 range. */
double umi_performance_clamp_unit(double value);
/* Calculate percentage change relative to a non-zero baseline. */
double umi_performance_percent_delta(double current, double baseline);
/* Calculate a safe ratio, returning zero when the denominator is zero. */
double umi_performance_safe_ratio(double numerator, double denominator);
/* Produce a deterministic FNV-1a fingerprint for evidence identifiers. */
uint64_t umi_performance_hash_text(const char *text);
/* Classify an observed value against warning and critical thresholds. */
UmiPerformanceSeverity umi_performance_severity_from_thresholds(double value, double warning_threshold, double critical_threshold);

#ifdef __cplusplus
}
#endif
#endif
