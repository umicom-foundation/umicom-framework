/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/workspace_runtime.h
 *
 * PURPOSE:
 *   Compose session, context, command and operation state into one toolkit-neutral application workspace runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_WORKSPACE_RUNTIME_H
#define UMICOM_APPLICATION_RUNTIME_WORKSPACE_RUNTIME_H

#include "umicom/application/runtime/command_surface.h"
#include "umicom/application/runtime/context_binding.h"
#include "umicom/application/runtime/operation.h"
#include "umicom/application/runtime/session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationWorkspaceRuntime {
    uint32_t structure_size;
    UmiApplicationSession session;
    UmiApplicationContextBindingStore contexts;
    UmiApplicationCommandSurface commands;
    UmiApplicationOperationLog operations;
} UmiApplicationWorkspaceRuntime;

UmiStatus umi_application_workspace_runtime_init(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationWorkspaceRuntime *out_runtime);
UmiStatus umi_application_workspace_runtime_select_layout(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *layout_id);
UmiStatus umi_application_workspace_runtime_activate_panel(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *panel_id);
UmiStatus umi_application_workspace_runtime_set_context(
    UmiApplicationWorkspaceRuntime *runtime,
    const char *group_id,
    const char *value);

#ifdef __cplusplus
}
#endif

#endif
