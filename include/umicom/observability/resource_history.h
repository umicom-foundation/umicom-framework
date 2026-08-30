/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/resource_history.h
 *
 * PURPOSE:
 *   Publish the public resource history contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_RESOURCE_HISTORY_H
#define UMICOM_OBSERVABILITY_RESOURCE_HISTORY_H
#include "umicom/observability/operations_contract.h"
typedef struct UmiOperationsResourceSample {
    char component_id[UMI_OPERATIONS_ID_CAPACITY];
    uint64_t timestamp_ns;
    double cpu_percent;
    uint64_t resident_memory_bytes;
    uint32_t thread_count;
    uint32_t open_handle_count;
    uint64_t queue_depth;
} UmiOperationsResourceSample;
typedef struct UmiOperationsResourceHistory { UmiOperationsResourceSample items[UMI_OPERATIONS_MAX_RESOURCE_SAMPLES]; size_t count; uint64_t revision; } UmiOperationsResourceHistory;
UmiStatus umi_operations_resource_history_add(UmiOperationsResourceHistory *history,const UmiOperationsResourceSample *sample);
const UmiOperationsResourceSample *umi_operations_resource_history_at(const UmiOperationsResourceHistory *history,size_t index);
const UmiOperationsResourceSample *umi_operations_resource_history_latest(const UmiOperationsResourceHistory *history,const char *component_id);
double umi_operations_resource_history_peak_cpu(const UmiOperationsResourceHistory *history,const char *component_id);
uint64_t umi_operations_resource_history_peak_memory(const UmiOperationsResourceHistory *history,const char *component_id);
#endif
