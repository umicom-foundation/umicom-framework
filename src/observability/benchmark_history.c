/* Umicom Framework | Benchmark history | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/benchmark_history.h"
#include <string.h>

UmiStatus umi_operations_benchmark_history_add(UmiOperationsBenchmarkHistory *history,const UmiOperationsBenchmarkRun *run)
{
    size_t index;
    if (history == NULL || run == NULL || run->benchmark_id[0] == '\0' || run->project_id[0] == '\0' || run->run_id[0] == '\0' || run->duration_ns == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < history->count; ++index) if (strcmp(history->items[index].run_id,run->run_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (history->count == UMI_OPERATIONS_MAX_BENCHMARKS) {
        (void)memmove(&history->items[0],&history->items[1],(history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }
    history->items[history->count++] = *run;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_at(const UmiOperationsBenchmarkHistory *history,size_t index)
{
    return history == NULL || index >= history->count ? NULL : &history->items[index];
}

const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_latest(const UmiOperationsBenchmarkHistory *history,const char *benchmark_id,const char *project_id)
{
    size_t index;
    if (history == NULL || benchmark_id == NULL || project_id == NULL) return NULL;
    for (index = history->count; index > 0U; --index) {
        const UmiOperationsBenchmarkRun *item = &history->items[index - 1U];
        if (strcmp(item->benchmark_id,benchmark_id) == 0 && strcmp(item->project_id,project_id) == 0) return item;
    }
    return NULL;
}

UmiStatus umi_operations_benchmark_compare(const UmiOperationsBenchmarkRun *baseline,const UmiOperationsBenchmarkRun *current,double stable_tolerance_percent,UmiOperationsBenchmarkComparison *out_comparison)
{
    double change;
    double magnitude;
    if (baseline == NULL || current == NULL || out_comparison == NULL || baseline->duration_ns == 0U || stable_tolerance_percent < 0.0 || strcmp(baseline->benchmark_id,current->benchmark_id) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    change = ((double)current->duration_ns - (double)baseline->duration_ns) * 100.0 / (double)baseline->duration_ns;
    magnitude = change < 0.0 ? -change : change;
    out_comparison->baseline_duration_ns = baseline->duration_ns;
    out_comparison->current_duration_ns = current->duration_ns;
    out_comparison->change_percent = change;
    if (magnitude <= stable_tolerance_percent) out_comparison->trend = UMI_OPERATIONS_TREND_STABLE;
    else if (change < 0.0) out_comparison->trend = UMI_OPERATIONS_TREND_IMPROVED;
    else out_comparison->trend = UMI_OPERATIONS_TREND_REGRESSED;
    return UMI_STATUS_OK;
}
