/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/route_preview.h
 *
 * PURPOSE:
 *   Preview context recipients before publication for UI confirmation and diagnostics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_ROUTE_PREVIEW_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_ROUTE_PREVIEW_H
#include "umicom/workbench_context_host/compatibility.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostRoutePreviewRow {
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    int compatibility_score;
    bool will_receive;
} UmiWorkbenchContextHostRoutePreviewRow;
typedef struct UmiWorkbenchContextHostRoutePreview {
    UmiWorkbenchContextHostRoutePreviewRow rows[UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS];
    size_t count;
    size_t recipient_count;
    uint64_t revision;
} UmiWorkbenchContextHostRoutePreview;
UmiStatus umi_workbench_context_host_route_preview_build(
    const UmiWorkbenchContextHost *host,const char *source_endpoint_id,
    const char *group_id,UmiContextKind kind,
    UmiWorkbenchContextHostRoutePreview *out_preview);
#ifdef __cplusplus
}
#endif
#endif
