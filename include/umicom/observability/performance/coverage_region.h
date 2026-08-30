/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/coverage_region.h
 *
 * PURPOSE:
 *   Represent code-coverage evidence, baselines and regressions for coverage region.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_COVERAGE_REGION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_COVERAGE_REGION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceCoverageRegion {
    uint32_t structure_size;
    uint32_t api_version;
    char id[UMI_PERFORMANCE_ID_CAPACITY];
    char subject_id[UMI_PERFORMANCE_ID_CAPACITY];
    UmiPerformanceState state;
    UmiPerformanceSeverity severity;
    uint64_t sequence;
    uint64_t timestamp_ns;
    double value;
    double auxiliary;
    uint64_t count;
    bool enabled;
} UmiPerformanceCoverageRegion;

/* Initialise a versioned coverage region record with stable identities. */
UmiStatus umi_performance_coverage_region_init(UmiPerformanceCoverageRegion *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_coverage_region_validate(const UmiPerformanceCoverageRegion *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_coverage_region_observe(UmiPerformanceCoverageRegion *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_coverage_region_same_identity(const UmiPerformanceCoverageRegion *left, const UmiPerformanceCoverageRegion *right);
/* Domain-specific policy helper for coverage region. */
double umi_performance_coverage_region_coverage_percent(uint64_t covered, uint64_t total);

#ifdef __cplusplus
}
#endif
#endif
