/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/snapshot.h
 *
 * PURPOSE:
 *   Project immutable host state for headless, GTK4 and future frontend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_SNAPSHOT_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_SNAPSHOT_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the workbench context host snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostSnapshot {
    uint32_t structure_size;
    char host_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char active_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    size_t endpoint_count;
    size_t inbox_count;
    size_t queued_delivery_count;
    size_t group_count;
    UmiWorkbenchContextHostMetrics metrics;
    bool suspended;
    uint64_t link_revision;
    uint64_t revision;
} UmiWorkbenchContextHostSnapshot;
/**
 * Provide the workbench context host snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_context_host_snapshot(
    const UmiWorkbenchContextHost *host,UmiWorkbenchContextHostSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
