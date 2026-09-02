/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/crash_registry.h
 *
 * PURPOSE:
 *   Publish the public crash registry contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_CRASH_REGISTRY_H
#define UMICOM_OBSERVABILITY_CRASH_REGISTRY_H
#include "umicom/observability/operations_contract.h"
/**
 * Represent the operations crash evidence data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsCrashEvidence {
    char crash_id[UMI_OPERATIONS_ID_CAPACITY];
    char application[UMI_OPERATIONS_ID_CAPACITY];
    char component_id[UMI_OPERATIONS_ID_CAPACITY];
    char reason[UMI_OPERATIONS_TEXT_CAPACITY];
    char report_reference[UMI_OPERATIONS_TEXT_CAPACITY];
    uint64_t timestamp_ns;
    uint64_t correlation_id;
    bool recovered;
} UmiOperationsCrashEvidence;
/**
 * Represent the operations crash registry data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsCrashRegistry { UmiOperationsCrashEvidence items[UMI_OPERATIONS_MAX_CRASHES]; size_t count; uint64_t revision; } UmiOperationsCrashRegistry;
/**
 * Add operations crash registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operations_crash_registry_add(UmiOperationsCrashRegistry *registry,const UmiOperationsCrashEvidence *evidence);
/**
 * Find operations crash registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiOperationsCrashEvidence *umi_operations_crash_registry_at(const UmiOperationsCrashRegistry *registry,size_t index);
/**
 * Find operations crash registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiOperationsCrashEvidence *umi_operations_crash_registry_find(const UmiOperationsCrashRegistry *registry,const char *crash_id);
/**
 * Provide the operations crash registry unrecovered operation used by this module and its
 * client applications.
 */
size_t umi_operations_crash_registry_unrecovered(const UmiOperationsCrashRegistry *registry);
/**
 * Provide the operations crash registry mark recovered operation used by this module and
 * its client applications.
 */
UmiStatus umi_operations_crash_registry_mark_recovered(UmiOperationsCrashRegistry *registry,const char *crash_id);
#endif
