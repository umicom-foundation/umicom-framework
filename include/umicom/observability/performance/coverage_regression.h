/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/coverage_regression.h
 *
 * PURPOSE:
 *   Represent code-coverage evidence, baselines and regressions for coverage regression.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_COVERAGE_REGRESSION_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_COVERAGE_REGRESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance coverage regression data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceCoverageRegression {
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
} UmiPerformanceCoverageRegression;

/* Initialise a versioned coverage regression record with stable identities. */
UmiStatus umi_performance_coverage_regression_init(UmiPerformanceCoverageRegression *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_coverage_regression_validate(const UmiPerformanceCoverageRegression *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_coverage_regression_observe(UmiPerformanceCoverageRegression *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_coverage_regression_same_identity(const UmiPerformanceCoverageRegression *left, const UmiPerformanceCoverageRegression *right);
/* Domain-specific policy helper for coverage regression. */
double umi_performance_coverage_regression_regression_percent(double current, double baseline);

#ifdef __cplusplus
}
#endif
#endif
