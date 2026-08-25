/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/benchmark_sample.h
 *
 * PURPOSE:
 *   Represent benchmark cases, runs, baselines and statistics for benchmark sample.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_SAMPLE_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_SAMPLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPerformanceBenchmarkSample {
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
} UmiPerformanceBenchmarkSample;

/* Initialise a versioned benchmark sample record with stable identities. */
UmiStatus umi_performance_benchmark_sample_init(UmiPerformanceBenchmarkSample *record, const char *id, const char *subject_id);
/* Validate structure/version/identity invariants before a record is consumed. */
UmiStatus umi_performance_benchmark_sample_validate(const UmiPerformanceBenchmarkSample *record);
/* Update point-in-time measurement evidence and monotonically advance sequence. */
UmiStatus umi_performance_benchmark_sample_observe(UmiPerformanceBenchmarkSample *record, double value, double auxiliary, uint64_t count, uint64_t timestamp_ns);
/* Compare stable identities without comparing volatile measurement state. */
bool umi_performance_benchmark_sample_same_identity(const UmiPerformanceBenchmarkSample *left, const UmiPerformanceBenchmarkSample *right);
/* Domain-specific policy helper for benchmark sample. */
double umi_performance_benchmark_sample_mean(double sum, uint64_t count);

#ifdef __cplusplus
}
#endif
#endif
