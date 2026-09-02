/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/benchmark_history.h
 *
 * PURPOSE:
 *   Publish the public benchmark history contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_BENCHMARK_HISTORY_H
#define UMICOM_OBSERVABILITY_BENCHMARK_HISTORY_H
#include "umicom/observability/operations_contract.h"
/**
 * Represent the operations benchmark run data shared with callers of this public contract.
 */
typedef struct UmiOperationsBenchmarkRun {
    char benchmark_id[UMI_OPERATIONS_ID_CAPACITY];
    char project_id[UMI_OPERATIONS_ID_CAPACITY];
    char run_id[UMI_OPERATIONS_ID_CAPACITY];
    uint64_t timestamp_ns;
    uint64_t duration_ns;
    double throughput;
    bool successful;
} UmiOperationsBenchmarkRun;
/**
 * Represent the operations benchmark comparison data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsBenchmarkComparison { uint64_t baseline_duration_ns; uint64_t current_duration_ns; double change_percent; UmiOperationsTrend trend; } UmiOperationsBenchmarkComparison;
/**
 * Represent the operations benchmark history data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsBenchmarkHistory { UmiOperationsBenchmarkRun items[UMI_OPERATIONS_MAX_BENCHMARKS]; size_t count; uint64_t revision; } UmiOperationsBenchmarkHistory;
/**
 * Add operations benchmark history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operations_benchmark_history_add(UmiOperationsBenchmarkHistory *history,const UmiOperationsBenchmarkRun *run);
/**
 * Find operations benchmark history while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_at(const UmiOperationsBenchmarkHistory *history,size_t index);
/**
 * Provide the operations benchmark history latest operation used by this module and its
 * client applications.
 */
const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_latest(const UmiOperationsBenchmarkHistory *history,const char *benchmark_id,const char *project_id);
/**
 * Provide the operations benchmark compare operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_benchmark_compare(const UmiOperationsBenchmarkRun *baseline,const UmiOperationsBenchmarkRun *current,double stable_tolerance_percent,UmiOperationsBenchmarkComparison *out_comparison);
#endif
