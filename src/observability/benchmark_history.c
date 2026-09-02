/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/benchmark_history.c
 *
 * PURPOSE:
 *   Implement the benchmark history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Benchmark history | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/benchmark_history.h"
#include <string.h>

/*
 * Add operations benchmark history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operations_benchmark_history_add(UmiOperationsBenchmarkHistory *history,const UmiOperationsBenchmarkRun *run)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || run == NULL || run->benchmark_id[0] == '\0' || run->project_id[0] == '\0' || run->run_id[0] == '\0' || run->duration_ns == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(history->items[index].run_id,run->run_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == UMI_OPERATIONS_MAX_BENCHMARKS) {
        (void)memmove(&history->items[0],&history->items[1],(history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }
    history->items[history->count++] = *run;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find operations benchmark history while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_at(const UmiOperationsBenchmarkHistory *history,size_t index)
{
    return history == NULL || index >= history->count ? NULL : &history->items[index];
}

/*
 * Provide the operations benchmark history latest operation used by this module and its
 * client applications.
 */
const UmiOperationsBenchmarkRun *umi_operations_benchmark_history_latest(const UmiOperationsBenchmarkHistory *history,const char *benchmark_id,const char *project_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || benchmark_id == NULL || project_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = history->count; index > 0U; --index) {
        const UmiOperationsBenchmarkRun *item = &history->items[index - 1U];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(item->benchmark_id,benchmark_id) == 0 && strcmp(item->project_id,project_id) == 0) return item;
    }
    return NULL;
}

/*
 * Provide the operations benchmark compare operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_benchmark_compare(const UmiOperationsBenchmarkRun *baseline,const UmiOperationsBenchmarkRun *current,double stable_tolerance_percent,UmiOperationsBenchmarkComparison *out_comparison)
{
    double change;
    double magnitude;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (baseline == NULL || current == NULL || out_comparison == NULL || baseline->duration_ns == 0U || stable_tolerance_percent < 0.0 || strcmp(baseline->benchmark_id,current->benchmark_id) != 0) return UMI_STATUS_INVALID_ARGUMENT;
    change = ((double)current->duration_ns - (double)baseline->duration_ns) * 100.0 / (double)baseline->duration_ns;
    magnitude = change < 0.0 ? -change : change;
    out_comparison->baseline_duration_ns = baseline->duration_ns;
    out_comparison->current_duration_ns = current->duration_ns;
    out_comparison->change_percent = change;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (magnitude <= stable_tolerance_percent) out_comparison->trend = UMI_OPERATIONS_TREND_STABLE;
    else /* Apply this branch only when its contract condition is satisfied. */ if (change < 0.0) out_comparison->trend = UMI_OPERATIONS_TREND_IMPROVED;
    /* Use this fallback path when the earlier condition does not apply. */
    else out_comparison->trend = UMI_OPERATIONS_TREND_REGRESSED;
    return UMI_STATUS_OK;
}
