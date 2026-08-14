/* Umicom Framework | Benchmark history | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_OBSERVABILITY_BENCHMARK_HISTORY_H
#define UMICOM_OBSERVABILITY_BENCHMARK_HISTORY_H
#include "umicom/observability/operations_contract.h"
typedef struct UmiOperationsBenchmarkRun {
    char benchmark_id[UMI_OPERATIONS_ID_CAPACITY];
    char project_id[UMI_OPERATIONS_ID_CAPACITY];
    char run_id[UMI_OPERATIONS_ID_CAPACITY];
    uint64_t timestamp_ns;
    uint64_t duration_ns;
    double throughput;
    bool successful;
} UmiOperationsBenchmarkRun;
typedef struct UmiOperationsBenchmarkComparison { uint64_t baseline_duration_ns; uint64_t current_duration_ns; double change_percent; UmiOperationsTrend trend; } UmiOperationsBenchmarkComparison;
typedef struct UmiOperationsBenchmarkHistory { UmiOperationsBenchmarkRun items[UMI_OPERATIONS_MAX_BENCHMARKS]; size_t count; uint64_t revision; } UmiOperationsBenchmarkHistory;
UmiStatus umi_operations_benchmark_history_add(UmiOperationsBenchmarkHistory *history,const UmiOperationsBenchmarkRun *run);
const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_at(const UmiOperationsBenchmarkHistory *history,size_t index);
const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_latest(const UmiOperationsBenchmarkHistory *history,const char *benchmark_id,const char *project_id);
UmiStatus umi_operations_benchmark_compare(const UmiOperationsBenchmarkRun *baseline,const UmiOperationsBenchmarkRun *current,double stable_tolerance_percent,UmiOperationsBenchmarkComparison *out_comparison);
#endif
