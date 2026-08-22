/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/toolbar_projection.h
 *
 * PURPOSE:
 *   Project navigation, pinning and active-group actions into a toolkit-neutral toolbar model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TOOLBAR_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TOOLBAR_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_TOOLBAR_ACTIONS 16U
typedef struct UmiWorkbenchContextHostToolbarAction {
    char action_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    char tooltip[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    UmiWorkbenchContextHostCommandKind command_kind;
    bool enabled;
} UmiWorkbenchContextHostToolbarAction;
typedef struct UmiWorkbenchContextHostToolbarProjection {
    UmiWorkbenchContextHostToolbarAction actions[UMI_WORKBENCH_CONTEXT_HOST_MAX_TOOLBAR_ACTIONS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchContextHostToolbarProjection;
UmiStatus umi_workbench_context_host_toolbar_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostToolbarProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
