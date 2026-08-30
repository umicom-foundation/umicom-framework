/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/suite_layout/gtk4_workstation.h
 *
 * PURPOSE:
 *   Bind canonical Application Suite layout selection to the existing GTK4
 *   workstation renderer for any thin Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_WORKSTATION_H
#define UMICOM_APPLICATION_SUITE_LAYOUT_GTK4_WORKSTATION_H

#include <gtk/gtk.h>

#include "umicom/application/suite_layout/runtime.h"
#include "umicom/application/suite_layout/selector_model.h"
#include "umicom/ui/gtk4/workstation/workspace_layout_host.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationSuiteGtk4WorkstationConfig {
    const char *application_id;
    const char *title;
    UmiGtk4WorkspaceLayoutPanelFactory panel_factory;
    void *user_data;
} UmiApplicationSuiteGtk4WorkstationConfig;

typedef struct UmiApplicationSuiteGtk4WorkstationSnapshot {
    char application_id[UMI_UI_ID_CAPACITY];
    char active_layout_id[UMI_UI_ID_CAPACITY];
    char active_layout_name[UMI_UI_TEXT_CAPACITY];
    size_t layout_count;
    size_t rendered_panel_count;
    size_t placeholder_count;
    size_t available_window_count;
    size_t recent_window_count;
    int layout_locked;
    int editing_layout;
    uint64_t revision;
} UmiApplicationSuiteGtk4WorkstationSnapshot;

typedef struct UmiApplicationSuiteGtk4Workstation
    UmiApplicationSuiteGtk4Workstation;

UmiStatus umi_application_suite_gtk4_workstation_create(
    const UmiApplicationSuiteGtk4WorkstationConfig *config,
    UmiApplicationSuiteGtk4Workstation **out_workstation);
void umi_application_suite_gtk4_workstation_destroy(
    UmiApplicationSuiteGtk4Workstation *workstation);
UmiStatus umi_application_suite_gtk4_workstation_select_layout(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *layout_id);
UmiStatus umi_application_suite_gtk4_workstation_begin_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation);
UmiStatus umi_application_suite_gtk4_workstation_commit_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation);
UmiStatus umi_application_suite_gtk4_workstation_cancel_layout_edit(
    UmiApplicationSuiteGtk4Workstation *workstation);
UmiStatus umi_application_suite_gtk4_workstation_open_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *tool_id,
    const char *group_id,
    int floating,
    uint64_t opened_at_ms,
    char *out_window_id,
    size_t out_window_id_capacity);
UmiStatus umi_application_suite_gtk4_workstation_move_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id,
    const char *group_id,
    double x,
    double y,
    double width,
    double height);
UmiStatus umi_application_suite_gtk4_workstation_close_window(
    UmiApplicationSuiteGtk4Workstation *workstation,
    const char *window_id);
GtkWidget *umi_application_suite_gtk4_workstation_widget(
    UmiApplicationSuiteGtk4Workstation *workstation);
UmiApplicationSuiteGtk4WorkstationSnapshot
umi_application_suite_gtk4_workstation_snapshot(
    const UmiApplicationSuiteGtk4Workstation *workstation);

#ifdef __cplusplus
}
#endif
#endif
