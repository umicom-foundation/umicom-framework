/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_productivity/workbench_bridge.h
 *
 * PURPOSE:
 *   Bind real source-control, terminal and Problems/output operations into the
 *   existing professional Developer Workbench command registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PRODUCTIVITY_WORKBENCH_BRIDGE_H
#define UMICOM_DEVELOPER_PRODUCTIVITY_WORKBENCH_BRIDGE_H

#include "umicom/developer_productivity/diagnostic_pipeline.h"
#include "umicom/developer_productivity/navigation_service.h"
#include "umicom/developer_productivity/output_channels.h"
#include "umicom/developer_productivity/problem_navigation.h"
#include "umicom/developer_productivity/source_control.h"
#include "umicom/developer_productivity/terminal_workspace.h"
#include "umicom/developer_project/project_workbench_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer productivity workbench bridge data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProductivityWorkbenchBridge
    UmiDeveloperProductivityWorkbenchBridge;

/**
 * Initialise developer productivity workbench bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_productivity_workbench_bridge_create(
    UmiDeveloperWorkbench *workbench,
    UmiDeveloperDiagnosticPipeline *diagnostics,
    UmiDeveloperNavigationService *navigation,
    UmiDeveloperOutputChannels *output,
    UmiDeveloperProductivityWorkbenchBridge **out_bridge);

/**
 * Release or reset state held by developer productivity workbench bridge so the same
 * storage can be reused safely.
 */
void umi_developer_productivity_workbench_bridge_destroy(
    UmiDeveloperProductivityWorkbenchBridge *bridge);

/**
 * Provide the developer productivity workbench bridge set source control operation used by
 * this module and its client applications.
 */
UmiStatus umi_developer_productivity_workbench_bridge_set_source_control(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    UmiDeveloperSourceControl *source_control);

/**
 * Provide the developer productivity workbench bridge set terminal operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_productivity_workbench_bridge_set_terminal(
    UmiDeveloperProductivityWorkbenchBridge *bridge,
    UmiDeveloperTerminalWorkspace *terminal);

/**
 * Provide the developer productivity workbench bridge bind operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_productivity_workbench_bridge_bind(
    UmiDeveloperProductivityWorkbenchBridge *bridge);

#ifdef __cplusplus
}
#endif

#endif
