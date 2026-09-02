/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/benchmark_statistics.h
 *
 * PURPOSE:
 *   Represent benchmark cases, runs, baselines and statistics for benchmark statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_STATISTICS_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_STATISTICS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance benchmark statistics data shared with callers of this public
 * contract.
 */
typedef struct UmiPerformanceBenchmarkStatistics {
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
} UmiPerformanceBenchmarkStatistics;

/* Initialise a versioned benchmark statistics record with stable identities. */
UmiStatus umi_performance_benchmark_statistics_init(UmiPerformanceBenchmarkStatistics *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_benchmark_statistics_validate(const UmiPerformanceBenchmarkStatistics *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_benchmark_statistics_observe(UmiPerformanceBenchmarkStatistics *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_benchmark_statistics_same_identity(const UmiPerformanceBenchmarkStatistics *left, const UmiPerformanceBenchmarkStatistics *right);
/* Domain-specific policy helper for benchmark statistics. */
double umi_performance_benchmark_statistics_mean(double sum, uint64_t count);

#ifdef __cplusplus
}
#endif
#endif
