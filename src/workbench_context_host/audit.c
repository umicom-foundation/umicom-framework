/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/audit.c
 *
 * PURPOSE:
 *   Implement bounded dynamic audit retention for operator-visible context-link evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/audit.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus grow(UmiWorkbenchContextHostAuditLog *log)
{
    UmiWorkbenchContextHostAuditEntry *items;
    size_t next;

    if (log->count < log->capacity) return UMI_STATUS_OK;
    if (log->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_AUDIT_ENTRIES) {
        memmove(
            &log->items[0],
            &log->items[1],
            (log->count - 1U) * sizeof(log->items[0]));
        --log->count;
        return UMI_STATUS_OK;
    }

    next = log->capacity == 0U ? 32U : log->capacity * 2U;
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_AUDIT_ENTRIES) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_AUDIT_ENTRIES;
    }
    items = (UmiWorkbenchContextHostAuditEntry *)realloc(
        log->items, next * sizeof(log->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (next > log->capacity) {
        memset(
            items + log->capacity,
            0,
            (next - log->capacity) * sizeof(items[0]));
    }
    log->items = items;
    log->capacity = next;
    return UMI_STATUS_OK;
}

void umi_workbench_context_host_audit_log_init(
    UmiWorkbenchContextHostAuditLog *log)
{
    if (log == NULL) return;
    memset(log, 0, sizeof(*log));
    log->revision = 1U;
}

void umi_workbench_context_host_audit_log_destroy(
    UmiWorkbenchContextHostAuditLog *log)
{
    if (log == NULL) return;
    free(log->items);
    memset(log, 0, sizeof(*log));
}

UmiStatus umi_workbench_context_host_audit_log_append(
    UmiWorkbenchContextHostAuditLog *log,
    const UmiWorkbenchContextHostAuditEntry *entry)
{
    UmiStatus status;
    if (log == NULL || entry == NULL ||
        entry->audit_id[0] == '\0' || entry->action[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = grow(log);
    if (status != UMI_STATUS_OK) return status;
    log->items[log->count++] = *entry;
    ++log->revision;
    return UMI_STATUS_OK;
}
