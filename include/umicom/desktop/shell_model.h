/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/shell_model.h
 *
 * PURPOSE:
 *   Expose a Master Controller-owned presentation model for the federated
 *   desktop: bottom layout tabs, multi-monitor canvas, selectable application
 *   windows and the visual layout designer. Frontends render this model only.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_SHELL_MODEL_H
#define UMICOM_DESKTOP_SHELL_MODEL_H

#include "umicom/desktop/layout_designer.h"
#include "umicom/desktop/monitor_canvas.h"
#include "umicom/desktop/shell_actions.h"

typedef struct UmiDesktopShellTab {
    char tab_id[UMI_DESKTOP_ID_CAPACITY];
    char layout_id[UMI_DESKTOP_ID_CAPACITY];
    char label[UMI_DESKTOP_TITLE_CAPACITY];
    char category[UMI_DESKTOP_ID_CAPACITY];
    bool active;
    bool pinned;
    bool dirty;
    bool closable;
} UmiDesktopShellTab;

typedef struct UmiDesktopShellSnapshot {
    char active_layout_id[UMI_DESKTOP_ID_CAPACITY];
    char active_layout_name[UMI_DESKTOP_TITLE_CAPACITY];
    char selected_window_id[UMI_DESKTOP_ID_CAPACITY];
    size_t tab_count;
    size_t monitor_count;
    size_t window_count;
    size_t context_group_count;
    bool designer_active;
    bool active_layout_locked;
    double viewport_width;
    double viewport_height;
    uint64_t revision;
} UmiDesktopShellSnapshot;

typedef struct UmiDesktopShellModel UmiDesktopShellModel;

UmiStatus umi_desktop_shell_model_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopShellModel **out_model);
void umi_desktop_shell_model_destroy(UmiDesktopShellModel *model);
UmiStatus umi_desktop_shell_model_set_viewport(
    UmiDesktopShellModel *model,
    double width,
    double height);
UmiStatus umi_desktop_shell_model_activate_layout(
    UmiDesktopShellModel *model,
    const char *layout_id);
UmiStatus umi_desktop_shell_model_begin_design(
    UmiDesktopShellModel *model,
    const char *working_layout_id,
    const char *working_name);
UmiStatus umi_desktop_shell_model_end_design(
    UmiDesktopShellModel *model,
    bool commit);
UmiStatus umi_desktop_shell_model_select_at(
    UmiDesktopShellModel *model,
    double canvas_x,
    double canvas_y);
UmiStatus umi_desktop_shell_model_move_selected_canvas(
    UmiDesktopShellModel *model,
    double canvas_delta_x,
    double canvas_delta_y);
UmiStatus umi_desktop_shell_model_snapshot(
    const UmiDesktopShellModel *model,
    UmiDesktopShellSnapshot *out_snapshot);
UmiStatus umi_desktop_shell_model_tab_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopShellTab *out_tab);
UmiStatus umi_desktop_shell_model_monitor_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopCanvasMonitor *out_monitor);
UmiStatus umi_desktop_shell_model_window_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopCanvasWindow *out_window);
UmiDesktopLayoutDesigner *umi_desktop_shell_model_designer(
    UmiDesktopShellModel *model);
UmiDesktopRuntime *umi_desktop_shell_model_runtime(UmiDesktopShellModel *model);

#endif
