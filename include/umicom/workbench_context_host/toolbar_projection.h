/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/toolbar_projection.h
 *
 * PURPOSE:
 *   Project navigation, pinning and active-group actions into a toolkit-neutral toolbar model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_TOOLBAR_PROJECTION_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_TOOLBAR_PROJECTION_H
#include "umicom/workbench_context_host/host.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WORKBENCH_CONTEXT_HOST_MAX_TOOLBAR_ACTIONS 16U
/**
 * Represent the workbench context host toolbar action data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostToolbarAction {
    char action_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_HOST_TITLE_CAPACITY];
    char tooltip[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    UmiWorkbenchContextHostCommandKind command_kind;
    bool enabled;
} UmiWorkbenchContextHostToolbarAction;
/**
 * Represent the workbench context host toolbar projection data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextHostToolbarProjection {
    UmiWorkbenchContextHostToolbarAction actions[UMI_WORKBENCH_CONTEXT_HOST_MAX_TOOLBAR_ACTIONS];
    size_t count;
    uint64_t revision;
} UmiWorkbenchContextHostToolbarProjection;
/**
 * Provide the workbench context host toolbar projection build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_toolbar_projection_build(
    const UmiWorkbenchContextHost *host,
    UmiWorkbenchContextHostToolbarProjection *out_projection);
#ifdef __cplusplus
}
#endif
#endif
