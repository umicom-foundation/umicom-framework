/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/inspector_projection.h
 *
 * PURPOSE:
 *   Project the active typed context into structured inspector rows without exposing mutable payload storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_INSPECTOR_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_INSPECTOR_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_INSPECTOR_ROWS 32U
typedef struct UmiWorkbenchContextHostInspectorRow {
    char name[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char value[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
} UmiWorkbenchContextHostInspectorRow;
typedef struct UmiWorkbenchContextHostInspectorProjection {
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char context_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char schema_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char source_application_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char source_panel_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiContextKind kind;
    UmiWorkbenchContextHostInspectorRow rows[UMI_WORKBENCH_CONTEXT_HOST_MAX_INSPECTOR_ROWS];
    size_t row_count;
    uint64_t revision;
    bool available;
} UmiWorkbenchContextHostInspectorProjection;
UmiStatus umi_workbench_context_host_inspector_projection_build(
    const UmiWorkbenchContextHost *host,const char *group_id,
    UmiWorkbenchContextHostInspectorProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
