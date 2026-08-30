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
typedef struct UmiOperationsCrashRegistry { UmiOperationsCrashEvidence items[UMI_OPERATIONS_MAX_CRASHES]; size_t count; uint64_t revision; } UmiOperationsCrashRegistry;
UmiStatus umi_operations_crash_registry_add(UmiOperationsCrashRegistry *registry,const UmiOperationsCrashEvidence *evidence);
const UmiOperationsCrashEvidence *umi_operations_crash_registry_at(const UmiOperationsCrashRegistry *registry,size_t index);
const UmiOperationsCrashEvidence *umi_operations_crash_registry_find(const UmiOperationsCrashRegistry *registry,const char *crash_id);
size_t umi_operations_crash_registry_unrecovered(const UmiOperationsCrashRegistry *registry);
UmiStatus umi_operations_crash_registry_mark_recovered(UmiOperationsCrashRegistry *registry,const char *crash_id);
#endif
