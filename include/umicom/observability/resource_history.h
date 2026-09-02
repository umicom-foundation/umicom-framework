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
/**
 * Represent the operations resource sample data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsResourceSample {
    char component_id[UMI_OPERATIONS_ID_CAPACITY];
    uint64_t timestamp_ns;
    double cpu_percent;
    uint64_t resident_memory_bytes;
    uint32_t thread_count;
    uint32_t open_handle_count;
    uint64_t queue_depth;
} UmiOperationsResourceSample;
/**
 * Represent the operations resource history data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsResourceHistory { UmiOperationsResourceSample items[UMI_OPERATIONS_MAX_RESOURCE_SAMPLES]; size_t count; uint64_t revision; } UmiOperationsResourceHistory;
/**
 * Add operations resource history only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operations_resource_history_add(UmiOperationsResourceHistory *history,const UmiOperationsResourceSample *sample);
/**
 * Find operations resource history while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiOperationsResourceSample *umi_operations_resource_history_at(const UmiOperationsResourceHistory *history,size_t index);
/**
 * Provide the operations resource history latest operation used by this module and its
 * client applications.
 */
const UmiOperationsResourceSample *umi_operations_resource_history_latest(const UmiOperationsResourceHistory *history,const char *component_id);
/**
 * Provide the operations resource history peak cpu operation used by this module and its
 * client applications.
 */
double umi_operations_resource_history_peak_cpu(const UmiOperationsResourceHistory *history,const char *component_id);
/**
 * Provide the operations resource history peak memory operation used by this module and
 * its client applications.
 */
uint64_t umi_operations_resource_history_peak_memory(const UmiOperationsResourceHistory *history,const char *component_id);
#endif
