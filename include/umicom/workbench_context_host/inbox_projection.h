/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/inbox_projection.h
 *
 * PURPOSE:
 *   Project pending context deliveries for diagnostics, testing and frontend badges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_INBOX_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_INBOX_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostInboxProjectionRow {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    size_t queued_count;
    uint64_t dropped_count;
    char next_context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiContextKind next_kind;
} UmiWorkbenchContextHostInboxProjectionRow;
typedef struct UmiWorkbenchContextHostInboxProjection {
    UmiWorkbenchContextHostInboxProjectionRow rows[UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS];
    size_t count;
    size_t total_queued;
    uint64_t total_dropped;
    uint64_t revision;
} UmiWorkbenchContextHostInboxProjection;
UmiStatus umi_workbench_context_host_inbox_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostInboxProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
