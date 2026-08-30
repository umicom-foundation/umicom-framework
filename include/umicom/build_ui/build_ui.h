/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build_ui/build_ui.h
 *
 * PURPOSE:
 *   Define toolkit-neutral Build Dashboard, Graph, History, Output, Artifact
 *   and Task view factories shared by every Umicom application frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_UI_BUILD_UI_H
#define UMICOM_BUILD_UI_BUILD_UI_H

#include "umicom/build/workspace.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_UI_VISIBLE_ROWS 24U
#define UMI_BUILD_UI_VIEW_DASHBOARD "umicom.build.workspace.dashboard"
#define UMI_BUILD_UI_VIEW_GRAPH "umicom.build.workspace.graph"
#define UMI_BUILD_UI_VIEW_HISTORY "umicom.build.workspace.history"
#define UMI_BUILD_UI_VIEW_OUTPUT "umicom.build.workspace.output"
#define UMI_BUILD_UI_VIEW_ARTIFACTS "umicom.build.workspace.artifacts"
#define UMI_BUILD_UI_VIEW_TASKS "umicom.build.workspace.tasks"

UmiStatus umi_build_ui_dashboard_view_create(
    const char *view_id,
    UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_build_ui_graph_view_create(
    const char *view_id,
    UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_build_ui_history_view_create(
    const char *view_id,
    UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_build_ui_output_view_create(
    const char *view_id,
    UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_build_ui_artifacts_view_create(
    const char *view_id,
    UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_build_ui_tasks_view_create(
    const char *view_id,
    UmiBuildWorkspace *workspace,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
