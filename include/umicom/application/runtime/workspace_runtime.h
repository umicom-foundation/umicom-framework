/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/workspace_runtime.h
 *
 * PURPOSE:
 *   Compose session, context, command and operation state into one toolkit-neutral application workspace runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_WORKSPACE_RUNTIME_H
#define UMICOM_APPLICATION_RUNTIME_WORKSPACE_RUNTIME_H

#include "umicom/application/runtime/command_surface.h"
#include "umicom/application/runtime/context_binding.h"
#include "umicom/application/runtime/operation.h"
#include "umicom/application/runtime/session.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application workspace runtime data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationWorkspaceRuntime {
    uint32_t structure_size;
    UmiApplicationSession session;
    UmiApplicationContextBindingStore contexts;
    UmiApplicationCommandSurface commands;
    UmiApplicationOperationLog operations;
    UmiUiWorkbench *workbench;
} UmiApplicationWorkspaceRuntime;

/**
 * Initialise application workspace runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_workspace_runtime_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationWorkspaceRuntime *out_runtime);
/**
 * Provide the application workspace runtime select layout operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_workspace_runtime_select_layout(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *layout_id);
/**
 * Provide the application workspace runtime activate panel operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_workspace_runtime_activate_panel(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *panel_id);
/* Remove one panel from the active application session and bound workbench. */
UmiStatus umi_application_workspace_runtime_deactivate_panel(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *panel_id);
/* Keep the application session and selected workspace profile lock in sync. */
UmiStatus umi_application_workspace_runtime_set_layout_locked(
    UmiApplicationWorkspaceRuntime *runtime,
    bool locked);
/**
 * Provide the application workspace runtime set context operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_workspace_runtime_set_context(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *group_id,
    const char *value);
/* Bind the product runtime to the existing Framework workbench. Once bound,
 * layout, panel and context changes are projected through canonical UI models. */
UmiStatus umi_application_workspace_runtime_bind_workbench(
    UmiApplicationWorkspaceRuntime *runtime,
    UmiUiWorkbench *workbench);
/* Detach the workbench without taking ownership or destroying either object. */
void umi_application_workspace_runtime_unbind_workbench(
    UmiApplicationWorkspaceRuntime *runtime);
/* Re-project the current session, layouts, panels and contexts into the bound workbench. */
UmiStatus umi_application_workspace_runtime_sync_workbench(
    UmiApplicationWorkspaceRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif
