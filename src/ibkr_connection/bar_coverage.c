/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/bar_coverage.c
 * PURPOSE: Summarise temporal discontinuities in bounded retained broker series.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/bar_coverage.h"
#include <string.h>
UmiStatus UmiBarCoverageAnalyze(const int64_t *timestamps, size_t count, uint32_t intervalMs,
                                UmiBarCoverage *out) {
  if (out == NULL || intervalMs == 0U || (count != 0U && timestamps == NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_BAR_COVERAGE_MAX_TIMESTAMPS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiBarCoverage value = {0};
  value.count = count;
  for (size_t i = 0U; i < count; ++i) {
    if (timestamps[i] < 0)
      return UMI_STATUS_INVALID_ARGUMENT;
    if (i == 0U) {
      value.first_ms = timestamps[i];
      continue;
    }
    /* A reversed or duplicate timestamp is an observation of bad ordering,
     * not a very large gap produced by wrapping unsigned arithmetic. */
    if (timestamps[i] <= timestamps[i - 1U]) {
      ++value.non_increasing_count;
      continue;
    }
    uint64_t delta = (uint64_t)timestamps[i] - (uint64_t)timestamps[i - 1U];
    if (delta % intervalMs != 0U)
      ++value.irregular_count;
    if (delta > intervalMs) {
      uint64_t slots = (delta - 1U) / intervalMs;
      if (slots > UINT64_MAX - value.unobserved_intervals)
        return UMI_STATUS_CAPACITY_EXCEEDED;
      ++value.gap_count;
      value.unobserved_intervals += slots;
      if (delta > value.largest_gap_ms)
        value.largest_gap_ms = delta;
    }
  }
  if (count != 0U)
    value.last_ms = timestamps[count - 1U];
  /* Publish after every count and overflow check. Hosts can keep a previous
   * quality report when an incomplete or malformed capture cannot be read. */
  *out = value;
  return UMI_STATUS_OK;
}
