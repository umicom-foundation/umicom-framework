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
/**
 * Represent the workbench context host audit entry data shared with callers of this public
 * contract.
 */
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
/**
 * Represent the workbench context host audit log data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostAuditLog {
    UmiWorkbenchContextHostAuditEntry *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextHostAuditLog;
/**
 * Initialise workbench context host audit log from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_audit_log_init(UmiWorkbenchContextHostAuditLog *log);
/**
 * Release or reset state held by workbench context host audit log so the same storage can
 * be reused safely.
 */
void umi_workbench_context_host_audit_log_destroy(UmiWorkbenchContextHostAuditLog *log);
/**
 * Add workbench context host audit log only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_context_host_audit_log_append(
    UmiWorkbenchContextHostAuditLog *log,const UmiWorkbenchContextHostAuditEntry *entry);
#ifdef __cplusplus
}
#endif
#endif
