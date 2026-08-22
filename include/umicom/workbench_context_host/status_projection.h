/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/status_projection.h
 *
 * PURPOSE:
 *   Project concise active-group and routing state for workbench status surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_STATUS_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_STATUS_PROJECTION_H
#include "umicom/workbench_context_host/snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkbenchContextHostStatusProjection {
    char active_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char active_group_title[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    char active_context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char summary[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    UmiContextChannelColour colour;
    UmiContextKind context_kind;
    size_t linked_endpoint_count;
    size_t queued_delivery_count;
    bool has_context;
    bool suspended;
    uint64_t revision;
} UmiWorkbenchContextHostStatusProjection;
UmiStatus umi_workbench_context_host_status_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostStatusProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
