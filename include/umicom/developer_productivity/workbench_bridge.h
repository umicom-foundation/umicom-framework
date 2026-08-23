/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/workbench_bridge.h
 *
 * PURPOSE:
 *   Bind real source-control, terminal and Problems/output operations into the
 *   existing professional Developer Workbench command registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_WORKBENCH_BRIDGE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_WORKBENCH_BRIDGE_H

#include "umicom/developer_productivity/diagnostic_pipeline.h"
#include "umicom/developer_productivity/navigation_service.h"
#include "umicom/developer_productivity/output_channels.h"
#include "umicom/developer_productivity/source_control.h"
#include "umicom/developer_productivity/terminal_workspace.h"
#include "umicom/developer_project/project_workbench_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProductivityWorkbenchBridge
    UmiDeveloperProductivityWorkbenchBridge;

UmiStatus umi_developer_productivity_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiDeveloperDiagnosticPipeline *diagnostics,
    UmiDeveloperNavigationService *navigation,
    UmiDeveloperOutputChannels *output,
    UmiDeveloperProductivityWorkbenchBridge **out_bridge);

void umi_developer_productivity_workbench_bridge_destroy(
    UmiDeveloperProductivityWorkbenchBridge *bridge);

UmiStatus umi_developer_productivity_workbench_bridge_set_source_control(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    UmiDeveloperSourceControl *source_control);

UmiStatus umi_developer_productivity_workbench_bridge_set_terminal(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    UmiDeveloperTerminalWorkspace *terminal);

UmiStatus umi_developer_productivity_workbench_bridge_bind(
    UmiDeveloperProductivityWorkbenchBridge *bridge);

#ifdef __cplusplus
}
#endif

#endif
