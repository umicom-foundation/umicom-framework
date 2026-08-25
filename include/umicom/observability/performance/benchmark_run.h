/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/benchmark_run.h
 *
 * PURPOSE:
 *   Represent benchmark cases, runs, baselines and statistics for benchmark run.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_RUN_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_RUN_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceBenchmarkRun {
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
} UmiPerformanceBenchmarkRun;

/* Initialise a versioned benchmark run record with stable identities. */
UmiStatus umi_performance_benchmark_run_init(UmiPerformanceBenchmarkRun *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_benchmark_run_validate(const UmiPerformanceBenchmarkRun *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_benchmark_run_observe(UmiPerformanceBenchmarkRun *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_benchmark_run_same_identity(const UmiPerformanceBenchmarkRun *left, const UmiPerformanceBenchmarkRun *right);
/* Domain-specific policy helper for benchmark run. */
uint64_t umi_performance_benchmark_run_duration_ns(uint64_t begin_ns, uint64_t end_ns);

#ifdef __cplusplus
}
#endif
#endif
