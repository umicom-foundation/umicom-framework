/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/observability/resource_history.c
 *
 * PURPOSE:
 *   Implement the resource history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Resource history | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/observability/resource_history.h"
#include <string.h>

/*
 * Add operations resource history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operations_resource_history_add(UmiOperationsResourceHistory *history,const UmiOperationsResourceSample *sample)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || sample == NULL || sample->component_id[0] == '\0' || sample->cpu_percent < 0.0 || sample->cpu_percent > 100.0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == UMI_OPERATIONS_MAX_RESOURCE_SAMPLES) {
        (void)memmove(&history->items[0],&history->items[1],(history->count - 1U) * sizeof(history->items[0]));
        history->count -= 1U;
    }
    history->items[history->count++] = *sample;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find operations resource history while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiOperationsResourceSample *umi_operations_resource_history_at(const UmiOperationsResourceHistory *history,size_t index)
{
    return history == NULL || index >= history->count ? NULL : &history->items[index];
}

/*
 * Provide the operations resource history latest operation used by this module and its
 * client applications.
 */
const UmiOperationsResourceSample *umi_operations_resource_history_latest(const UmiOperationsResourceHistory *history,const char *component_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || component_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = history->count; index > 0U; --index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(history->items[index - 1U].component_id,component_id) == 0) return &history->items[index - 1U];
    return NULL;
}

/*
 * Provide the operations resource history peak cpu operation used by this module and its
 * client applications.
 */
double umi_operations_resource_history_peak_cpu(const UmiOperationsResourceHistory *history,const char *component_id)
{
    double peak = 0.0;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || component_id == NULL) return peak;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(history->items[index].component_id,component_id) == 0 && history->items[index].cpu_percent > peak) peak = history->items[index].cpu_percent;
    return peak;
}

/*
 * Provide the operations resource history peak memory operation used by this module and
 * its client applications.
 */
uint64_t umi_operations_resource_history_peak_memory(const UmiOperationsResourceHistory *history,const char *component_id)
{
    uint64_t peak = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || component_id == NULL) return peak;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(history->items[index].component_id,component_id) == 0 && history->items[index].resident_memory_bytes > peak) peak = history->items[index].resident_memory_bytes;
    return peak;
}
