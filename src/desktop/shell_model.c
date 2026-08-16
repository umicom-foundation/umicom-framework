/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/shell_model.c
 *
 * PURPOSE:
 *   Implement the toolkit-neutral federated desktop presentation owned by the
 *   Master Controller and consumed by every application shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/shell_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDesktopShellModel {
    UmiDesktopRuntime *runtime;
    UmiDesktopLayoutDesigner *designer;
    UmiDesktopMonitorCanvas canvas;
    double viewport_width;
    double viewport_height;
    double padding;
    uint64_t revision;
};

static UmiStatus rebuild_canvas(UmiDesktopShellModel *model)
{
    return umi_desktop_monitor_canvas_build(
        umi_desktop_runtime_monitors(model->runtime),
        model->viewport_width, model->viewport_height, model->padding,
        &model->canvas);
}

UmiStatus umi_desktop_shell_model_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopShellModel **out_model)
{
    UmiDesktopShellModel *model;
    UmiStatus status;
    if (runtime == NULL || out_model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiDesktopShellModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->runtime = runtime;
    model->viewport_width = 1024.0;
    model->viewport_height = 640.0;
    model->padding = 24.0;
    status = umi_desktop_layout_designer_create(runtime, &model->designer);
    if (status == UMI_STATUS_OK) status = rebuild_canvas(model);
    if (status != UMI_STATUS_OK) {
        umi_desktop_layout_designer_destroy(model->designer);
        free(model);
        return status;
    }
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_desktop_shell_model_destroy(UmiDesktopShellModel *model)
{
    if (model == NULL) return;
    umi_desktop_layout_designer_destroy(model->designer);
    free(model);
}

UmiStatus umi_desktop_shell_model_set_viewport(
    UmiDesktopShellModel *model,
    double width,
    double height)
{
    UmiStatus status;
    if (model == NULL || width <= 0.0 || height <= 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    model->viewport_width = width;
    model->viewport_height = height;
    status = rebuild_canvas(model);
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shell_model_activate_layout(
    UmiDesktopShellModel *model,
    const char *layout_id)
{
    UmiDesktopLayoutDesignerSnapshot designer;
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_layout_designer_snapshot(model->designer, &designer);
    if (status != UMI_STATUS_OK) return status;
    if (designer.active) return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_runtime_activate_layout(model->runtime, layout_id);
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shell_model_begin_design(
    UmiDesktopShellModel *model,
    const char *working_layout_id,
    const char *working_name)
{
    UmiDesktopSnapshot desktop;
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_runtime_snapshot(model->runtime, &desktop);
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_layout_designer_begin(
            model->designer, desktop.active_layout_id, working_layout_id,
            working_name);
    }
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shell_model_end_design(
    UmiDesktopShellModel *model,
    bool commit)
{
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = commit
        ? umi_desktop_layout_designer_commit(model->designer)
        : umi_desktop_layout_designer_cancel(model->designer);
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shell_model_select_at(
    UmiDesktopShellModel *model,
    double canvas_x,
    double canvas_y)
{
    UmiDesktopWindowManager *windows;
    UmiDesktopLayoutDesignerSnapshot designer;
    size_t index;
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_layout_designer_snapshot(model->designer, &designer);
    if (status != UMI_STATUS_OK || !designer.active)
        return UMI_STATUS_INVALID_STATE;
    windows = umi_desktop_runtime_windows(model->runtime);
    for (index = windows->count; index > 0U; --index) {
        const UmiDesktopWindow *window = &windows->windows[index - 1U];
        UmiDesktopCanvasWindow projected;
        if (!window->visible) continue;
        status = umi_desktop_monitor_canvas_project_window(
            &model->canvas, window, false, false, &projected);
        if (status != UMI_STATUS_OK) return status;
        if (umi_desktop_canvas_rect_contains(
                projected.bounds, canvas_x, canvas_y)) {
            status = umi_desktop_layout_designer_select(
                model->designer, window->window_id);
            if (status == UMI_STATUS_OK) model->revision += 1U;
            return status;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_desktop_shell_model_move_selected_canvas(
    UmiDesktopShellModel *model,
    double canvas_delta_x,
    double canvas_delta_y)
{
    int32_t desktop_delta_x;
    int32_t desktop_delta_y;
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_monitor_canvas_delta_to_desktop(
        &model->canvas, canvas_delta_x, canvas_delta_y,
        &desktop_delta_x, &desktop_delta_y);
    if (status == UMI_STATUS_OK) {
        status = umi_desktop_layout_designer_move_selected(
            model->designer, desktop_delta_x, desktop_delta_y);
    }
    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

UmiStatus umi_desktop_shell_model_snapshot(
    const UmiDesktopShellModel *model,
    UmiDesktopShellSnapshot *out_snapshot)
{
    UmiDesktopSnapshot desktop;
    UmiDesktopLayoutDesignerSnapshot designer;
    const UmiDesktopLayout *layout;
    int first;
    int second;
    int third = 0;
    UmiStatus status;
    if (model == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_runtime_snapshot(model->runtime, &desktop);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_designer_snapshot(model->designer,
                                                       &designer);
    if (status != UMI_STATUS_OK) return status;
    layout = umi_desktop_layout_catalogue_find(
        umi_desktop_runtime_layouts(model->runtime), desktop.active_layout_id);
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    first = snprintf(out_snapshot->active_layout_id,
                     sizeof(out_snapshot->active_layout_id), "%s",
                     desktop.active_layout_id);
    second = snprintf(out_snapshot->active_layout_name,
                      sizeof(out_snapshot->active_layout_name), "%s",
                      layout->name);
    if (designer.selected_window_id[0] != '\0') {
        third = snprintf(out_snapshot->selected_window_id,
                         sizeof(out_snapshot->selected_window_id), "%s",
                         designer.selected_window_id);
    }
    if (first < 0 || second < 0 || third < 0 ||
        (size_t)first >= sizeof(out_snapshot->active_layout_id) ||
        (size_t)second >= sizeof(out_snapshot->active_layout_name) ||
        (size_t)third >= sizeof(out_snapshot->selected_window_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_snapshot->tab_count = desktop.tab_count;
    out_snapshot->monitor_count = desktop.monitor_count;
    out_snapshot->window_count = desktop.window_count;
    out_snapshot->context_group_count = desktop.context_group_count;
    out_snapshot->designer_active = designer.active;
    out_snapshot->active_layout_locked = layout->locked;
    out_snapshot->viewport_width = model->viewport_width;
    out_snapshot->viewport_height = model->viewport_height;
    out_snapshot->revision = desktop.revision + designer.revision +
        model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shell_model_tab_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopShellTab *out_tab)
{
    UmiDesktopLayoutTabs *tabs;
    const UmiDesktopLayout *layout;
    const UmiDesktopLayoutTab *tab;
    int first;
    int second;
    int third;
    int fourth;
    if (model == NULL || out_tab == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    tabs = umi_desktop_runtime_tabs(model->runtime);
    if (index >= tabs->count) return UMI_STATUS_NOT_FOUND;
    tab = &tabs->tabs[index];
    layout = umi_desktop_layout_catalogue_find(
        umi_desktop_runtime_layouts(model->runtime), tab->layout_id);
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_tab, 0, sizeof(*out_tab));
    first = snprintf(out_tab->tab_id, sizeof(out_tab->tab_id), "%s",
                     tab->tab_id);
    second = snprintf(out_tab->layout_id, sizeof(out_tab->layout_id), "%s",
                      tab->layout_id);
    third = snprintf(out_tab->label, sizeof(out_tab->label), "%s", tab->label);
    fourth = snprintf(out_tab->category, sizeof(out_tab->category), "%s",
                      layout->category);
    if (first < 0 || second < 0 || third < 0 || fourth < 0 ||
        (size_t)first >= sizeof(out_tab->tab_id) ||
        (size_t)second >= sizeof(out_tab->layout_id) ||
        (size_t)third >= sizeof(out_tab->label) ||
        (size_t)fourth >= sizeof(out_tab->category))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_tab->active = tab->active;
    out_tab->pinned = tab->pinned;
    out_tab->dirty = tab->dirty;
    out_tab->closable = tab->closable;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shell_model_monitor_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopCanvasMonitor *out_monitor)
{
    UmiDesktopMonitorTopology *topology;
    if (model == NULL || out_monitor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    topology = umi_desktop_runtime_monitors(model->runtime);
    if (index >= topology->count) return UMI_STATUS_NOT_FOUND;
    return umi_desktop_monitor_canvas_project_monitor(
        &model->canvas, &topology->monitors[index], out_monitor);
}

UmiStatus umi_desktop_shell_model_window_at(
    const UmiDesktopShellModel *model,
    size_t index,
    UmiDesktopCanvasWindow *out_window)
{
    UmiDesktopWindowManager *windows;
    UmiDesktopLayoutDesignerSnapshot designer;
    const UmiDesktopLayout *layout;
    UmiDesktopSnapshot desktop;
    UmiStatus status;
    if (model == NULL || out_window == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    windows = umi_desktop_runtime_windows(model->runtime);
    if (index >= windows->count) return UMI_STATUS_NOT_FOUND;
    status = umi_desktop_layout_designer_snapshot(model->designer, &designer);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_runtime_snapshot(model->runtime, &desktop);
    if (status != UMI_STATUS_OK) return status;
    layout = umi_desktop_layout_catalogue_find(
        umi_desktop_runtime_layouts(model->runtime), desktop.active_layout_id);
    return umi_desktop_monitor_canvas_project_window(
        &model->canvas, &windows->windows[index],
        strcmp(windows->windows[index].window_id,
               designer.selected_window_id) == 0,
        layout != NULL && layout->locked, out_window);
}

UmiDesktopLayoutDesigner *umi_desktop_shell_model_designer(
    UmiDesktopShellModel *model)
{
    return model != NULL ? model->designer : NULL;
}

UmiDesktopRuntime *umi_desktop_shell_model_runtime(UmiDesktopShellModel *model)
{
    return model != NULL ? model->runtime : NULL;
}
