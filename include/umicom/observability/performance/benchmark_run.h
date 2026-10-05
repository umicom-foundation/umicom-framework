/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/performance/benchmark_run.h
 *
 * PURPOSE:
 *   Represent benchmark cases, runs, baselines and statistics for benchmark run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_RUN_H
#define UMICOM_OBSERVABILITY_PERFORMANCE_BENCHMARK_RUN_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/observability/performance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the performance benchmark run data shared with callers of this public
 * contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_performance_benchmark_run_archive_encode(const UmiPerformanceBenchmarkRun *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_performance_benchmark_run_archive_decode(const void *bytes, size_t byte_count,
    UmiPerformanceBenchmarkRun *value);

#ifdef __cplusplus
}
#endif
#endif
