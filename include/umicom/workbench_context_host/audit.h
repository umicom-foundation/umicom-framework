/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/audit.h
 *
 * PURPOSE:
 *   Record context-link assignment, publication, navigation and policy decisions for audit surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_AUDIT_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_AUDIT_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_AUDIT_ENTRIES 512U
typedef struct UmiWorkbenchContextHostAuditEntry {
    char audit_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char action[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiStatus status;
    uint64_t timestamp_ms;
} UmiWorkbenchContextHostAuditEntry;
typedef struct UmiWorkbenchContextHostAuditLog {
    UmiWorkbenchContextHostAuditEntry *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostAuditLog;
void umi_workbench_context_host_audit_log_init(UmiWorkbenchContextHostAuditLog *log);
void umi_workbench_context_host_audit_log_destroy(UmiWorkbenchContextHostAuditLog *log);
UmiStatus umi_workbench_context_host_audit_log_append(
    UmiWorkbenchContextHostAuditLog *log,const UmiWorkbenchContextHostAuditEntry *entry);
#ifdef __cplusplus
}
#endif
#endif
