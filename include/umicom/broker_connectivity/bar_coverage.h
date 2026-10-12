/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/bar_coverage.h
 * PURPOSE: Describe timestamp coverage without treating market closures as feed failures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_BAR_COVERAGE_H
#define UMICOM_BROKER_CONNECTIVITY_BAR_COVERAGE_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_BAR_COVERAGE_MAX_TIMESTAMPS 4096U
typedef struct UmiBarCoverage {
  size_t count, gap_count, irregular_count, non_increasing_count;
  uint64_t unobserved_intervals, largest_gap_ms;
  int64_t first_ms, last_ms;
} UmiBarCoverage;
/** @brief Describe a retained timestamp series using an explicit positive interval.
 * Times must be nonnegative milliseconds; count is bounded to 4096. Empty input
 * accepts NULL. Failure retains out. A gap is a positive step larger than the
 * expected interval; irregular means not an exact interval multiple.
 * Unobserved intervals are inferred slots strictly before the next timestamp.
 * Duplicate/backward steps are counted without unsigned subtraction. No sorting,
 * interpolation, session-calendar assumption or missing-price invention occurs.
 * These observations do not establish feed loss, freshness or trading safety. */
UmiStatus UmiBarCoverageAnalyze(const int64_t *timestamps, size_t count, uint32_t intervalMs,
                                UmiBarCoverage *out);
#ifdef __cplusplus
}
#endif
#endif
