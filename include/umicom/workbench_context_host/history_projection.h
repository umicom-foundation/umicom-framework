/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/history_projection.h
 *
 * PURPOSE:
 *   Project recent context history into a compact reverse-chronological model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_HISTORY_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_HISTORY_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_HISTORY_PROJECTION_LIMIT 64U
typedef struct UmiWorkbenchContextHostHistoryProjectionRow {
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char source_application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiContextKind kind;
    uint64_t sequence;
    uint64_t published_at_ms;
} UmiWorkbenchContextHostHistoryProjectionRow;
typedef struct UmiWorkbenchContextHostHistoryProjection {
    UmiWorkbenchContextHostHistoryProjectionRow rows[UMI_WORKBENCH_CONTEXT_HOST_HISTORY_PROJECTION_LIMIT];
    size_t count;
    uint64_t revision;
} UmiWorkbenchContextHostHistoryProjection;
UmiStatus umi_workbench_context_host_history_projection_build(
    const UmiWorkbenchContextHost *host,const char *group_id,
    UmiWorkbenchContextHostHistoryProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
