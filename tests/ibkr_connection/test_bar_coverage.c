/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_bar_coverage.c
 * PURPOSE: Check timestamp gap arithmetic without network access or market assumptions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/bar_coverage.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                              \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(void) {
  UmiBarCoverage report = {0};
  const int64_t regular[] = {0, 5000, 10000};
  CHECK(UmiBarCoverageAnalyze(regular, 3U, 5000U, &report) == UMI_STATUS_OK);
  CHECK(report.count == 3U && report.gap_count == 0U && report.last_ms == 10000);
  const int64_t gaps[] = {0, 5000, 20000, 27500};
  CHECK(UmiBarCoverageAnalyze(gaps, 4U, 5000U, &report) == UMI_STATUS_OK);
  CHECK(report.gap_count == 2U && report.unobserved_intervals == 3U &&
        report.irregular_count == 1U && report.largest_gap_ms == 15000U);
  const int64_t reversed[] = {10000, 10000, 5000};
  CHECK(UmiBarCoverageAnalyze(reversed, 3U, 5000U, &report) == UMI_STATUS_OK);
  CHECK(report.non_increasing_count == 2U && report.gap_count == 0U);
  unsigned char previous[sizeof(report)];
  memcpy(previous, &report, sizeof(report));
  const int64_t invalid[] = {0, -1};
  CHECK(UmiBarCoverageAnalyze(invalid, 2U, 5000U, &report) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(memcmp(&report, previous, sizeof(report)) == 0);
  const int64_t large[] = {0, INT64_MAX};
  CHECK(UmiBarCoverageAnalyze(large, 2U, 1U, &report) == UMI_STATUS_OK);
  CHECK(report.unobserved_intervals == (uint64_t)INT64_MAX - 1U);
  CHECK(UmiBarCoverageAnalyze(NULL, 0U, 5000U, &report) == UMI_STATUS_OK && report.count == 0U);
  CHECK(UmiBarCoverageAnalyze(NULL, 1U, 5000U, &report) == UMI_STATUS_INVALID_ARGUMENT);
  CHECK(UmiBarCoverageAnalyze(regular, UMI_BAR_COVERAGE_MAX_TIMESTAMPS + 1U, 5000U, &report) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
  /* Repeated extreme forward jumps must fail rather than wrapping the count. */
  const int64_t overflowing[] = {0, INT64_MAX, 0, INT64_MAX, 0, INT64_MAX};
  memcpy(previous, &report, sizeof(report));
  CHECK(UmiBarCoverageAnalyze(overflowing, 6U, 1U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
  CHECK(memcmp(&report, previous, sizeof(report)) == 0);
  return 0;
}
