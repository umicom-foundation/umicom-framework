/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/shell_model.c
 *
 * PURPOSE:
 *   Implement the toolkit-neutral federated desktop presentation owned by the
 *   Master Controller and consumed by every application shell.
 *
 * Repository Mega Batch R2 adds the Framework-owned application taskbar. The
 * taskbar projects validated application definitions and presentation metadata;
 * it does not scan arbitrary folders or execute untrusted files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/shell_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/application.h"

struct UmiDesktopShellModel {
    UmiDesktopRuntime *runtime;
    UmiDesktopLayoutDesigner *designer;
    UmiDesktopMonitorCanvas canvas;
    UmiDesktopTaskbarItem applications[UMI_DESKTOP_TASKBAR_MAX_APPLICATIONS];
    size_t application_count;
    char active_application_id[UMI_DESKTOP_ID_CAPACITY];
    double viewport_width;
    double viewport_height;
    double padding;
    uint64_t revision;
};

static const UmiApplicationPresentation GENERIC_PRESENTATION = {
    sizeof(UmiApplicationPresentation),
    "org.umicom.generic",
    "umicom.icon.application.generic",
    "mosaic",
    "applications",
    UMI_APPLICATION_ENTRY_WORKBENCH,
    false,
    false
};

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus rebuild_canvas(UmiDesktopShellModel *model)
{
    return umi_desktop_monitor_canvas_build(
        umi_desktop_runtime_monitors(model->runtime),
        model->viewport_width, model->viewport_height, model->padding,
        &model->canvas);
}

static UmiDesktopTaskbarItem *find_application_mutable(
    UmiDesktopShellModel *model,
    const char *application_id)
{
    size_t index;
    if (model == NULL || application_id == NULL || application_id[0] == '\0')
        return NULL;
    for (index = 0U; index < model->application_count; ++index) {
        if (strcmp(model->applications[index].application_id,
                   application_id) == 0)
            return &model->applications[index];
    }
    return NULL;
}

static const UmiDesktopTaskbarItem *find_application_const(
    const UmiDesktopShellModel *model,
    const char *application_id)
{
    size_t index;
    if (model == NULL || application_id == NULL || application_id[0] == '\0')
        return NULL;
    for (index = 0U; index < model->application_count; ++index) {
        if (strcmp(model->applications[index].application_id,
                   application_id) == 0)
            return &model->applications[index];
    }
    return NULL;
}

static UmiStatus initialise_taskbar(UmiDesktopShellModel *model)
{
    const size_t count = umi_application_portfolio_count();
    size_t index;
    bool selected_active = false;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_DESKTOP_TASKBAR_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_application_resource_catalogue_validate() != UMI_STATUS_OK ||
        umi_application_presentation_catalogue_validate() != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;

    model->application_count = count;
    for (index = 0U; index < count; ++index) {
        const UmiApplicationDefinition *definition =
            umi_application_portfolio_at(index);
        const UmiApplicationPresentation *presentation;
        const UmiApplicationResourceDescriptor *icon;
        UmiDesktopTaskbarItem *item = &model->applications[index];
        UmiStatus status;
        if (definition == NULL ||
            umi_application_definition_validate(definition) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        presentation = umi_application_presentation_find(
            definition->application_id);
        if (presentation == NULL) presentation = &GENERIC_PRESENTATION;
        icon = umi_application_resource_catalogue_find(
            presentation->icon_resource_id);
        if (icon == NULL || icon->kind != UMI_APPLICATION_RESOURCE_THEME_ICON)
            icon = umi_application_resource_catalogue_find(
                "umicom.icon.application.generic");
        if (icon == NULL) return UMI_STATUS_INVALID_STATE;

        (void)memset(item, 0, sizeof(*item));
        status = copy_text(item->application_id,
                           sizeof(item->application_id),
                           definition->application_id);
        if (status == UMI_STATUS_OK)
            status = copy_text(item->display_name,
                               sizeof(item->display_name),
                               definition->display_name);
        if (status == UMI_STATUS_OK)
            status = copy_text(item->executable_name,
                               sizeof(item->executable_name),
                               definition->executable_name);
        if (status == UMI_STATUS_OK)
            status = copy_text(item->icon_resource_id,
                               sizeof(item->icon_resource_id),
                               presentation->icon_resource_id);
        if (status == UMI_STATUS_OK)
            status = copy_text(item->icon_name,
                               sizeof(item->icon_name), icon->locator);
        if (status == UMI_STATUS_OK)
            status = copy_text(item->default_layout_id,
                               sizeof(item->default_layout_id),
                               presentation->default_layout_id);
        if (status == UMI_STATUS_OK)
            status = copy_text(item->taskbar_group,
                               sizeof(item->taskbar_group),
                               presentation->taskbar_group);
        if (status != UMI_STATUS_OK) return status;

        item->family = definition->family;
        item->maturity = definition->maturity;
        item->entry_kind = presentation->entry_kind;
        item->installed = definition->maturity == UMI_APPLICATION_AVAILABLE;
        item->compatible = item->installed;
        item->enabled = item->installed;
        item->visible = item->installed;
        item->pinned = item->installed && presentation->pinned_by_default;
        item->state = item->installed
            ? UMI_DESKTOP_APPLICATION_STOPPED
            : UMI_DESKTOP_APPLICATION_UNAVAILABLE;
        item->revision = 1U;

        /* The first available product represents the process hosting this
         * reference shell. Future launchers can replace this initial presence
         * with package/runtime discovery before exposing the taskbar. */
        if (item->visible && !selected_active) {
            item->running = true;
            item->active = true;
            item->state = UMI_DESKTOP_APPLICATION_RUNNING;
            status = copy_text(model->active_application_id,
                               sizeof(model->active_application_id),
                               item->application_id);
            if (status != UMI_STATUS_OK) return status;
            selected_active = true;
        }
    }
    return UMI_STATUS_OK;
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
    if (status == UMI_STATUS_OK) status = initialise_taskbar(model);
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

UmiStatus umi_desktop_shell_model_set_application_presence(
    UmiDesktopShellModel *model,
    const char *application_id,
    bool installed,
    bool compatible,
    bool enabled)
{
    UmiDesktopTaskbarItem *item = find_application_mutable(model, application_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    item->installed = installed;
    item->compatible = compatible;
    item->enabled = enabled;
    item->visible = installed && compatible && enabled;
    if (!item->visible) {
        item->running = false;
        item->active = false;
        item->attention = false;
        item->state = installed
            ? UMI_DESKTOP_APPLICATION_STOPPED
            : UMI_DESKTOP_APPLICATION_UNAVAILABLE;
        if (strcmp(model->active_application_id, item->application_id) == 0)
            model->active_application_id[0] = '\0';
    } else if (item->state == UMI_DESKTOP_APPLICATION_UNAVAILABLE) {
        item->state = UMI_DESKTOP_APPLICATION_STOPPED;
    }
    item->revision += 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shell_model_set_application_state(
    UmiDesktopShellModel *model,
    const char *application_id,
    UmiDesktopApplicationState state)
{
    UmiDesktopTaskbarItem *item = find_application_mutable(model, application_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (state < UMI_DESKTOP_APPLICATION_UNAVAILABLE ||
        state > UMI_DESKTOP_APPLICATION_FAILED)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!item->installed && state != UMI_DESKTOP_APPLICATION_UNAVAILABLE)
        return UMI_STATUS_INVALID_STATE;
    item->state = state;
    item->attention = state == UMI_DESKTOP_APPLICATION_ATTENTION;
    item->running = state == UMI_DESKTOP_APPLICATION_RUNNING ||
                    state == UMI_DESKTOP_APPLICATION_ATTENTION;
    if (!item->running && item->active) {
        item->active = false;
        model->active_application_id[0] = '\0';
    }
    item->revision += 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shell_model_pin_application(
    UmiDesktopShellModel *model,
    const char *application_id,
    bool pinned)
{
    UmiDesktopTaskbarItem *item = find_application_mutable(model, application_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    item->pinned = pinned;
    item->revision += 1U;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shell_model_activate_application(
    UmiDesktopShellModel *model,
    const char *application_id)
{
    UmiDesktopTaskbarItem *item;
    size_t index;
    UmiStatus status;
    if (model == NULL || application_id == NULL || application_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    item = find_application_mutable(model, application_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    if (!item->installed || !item->compatible || !item->enabled || !item->visible)
        return UMI_STATUS_UNAVAILABLE;
    for (index = 0U; index < model->application_count; ++index)
        model->applications[index].active = false;
    item->active = true;
    item->running = true;
    item->attention = false;
    item->state = UMI_DESKTOP_APPLICATION_RUNNING;
    item->revision += 1U;
    status = copy_text(model->active_application_id,
                       sizeof(model->active_application_id),
                       item->application_id);
    if (status != UMI_STATUS_OK) return status;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_shell_model_application_at(
    const UmiDesktopShellModel *model,
    size_t visible_index,
    UmiDesktopTaskbarItem *out_item)
{
    size_t index;
    size_t visible = 0U;
    if (model == NULL || out_item == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->application_count; ++index) {
        if (!model->applications[index].visible) continue;
        if (visible == visible_index) {
            *out_item = model->applications[index];
            return UMI_STATUS_OK;
        }
        visible += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_desktop_shell_model_find_application(
    const UmiDesktopShellModel *model,
    const char *application_id,
    UmiDesktopTaskbarItem *out_item)
{
    const UmiDesktopTaskbarItem *item;
    if (out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    item = find_application_const(model, application_id);
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_item = *item;
    return UMI_STATUS_OK;
}

const char *umi_desktop_application_state_text(
    UmiDesktopApplicationState state)
{
    switch (state) {
    case UMI_DESKTOP_APPLICATION_UNAVAILABLE: return "unavailable";
    case UMI_DESKTOP_APPLICATION_STOPPED: return "stopped";
    case UMI_DESKTOP_APPLICATION_STARTING: return "starting";
    case UMI_DESKTOP_APPLICATION_RUNNING: return "running";
    case UMI_DESKTOP_APPLICATION_ATTENTION: return "attention";
    case UMI_DESKTOP_APPLICATION_FAILED: return "failed";
    default: return "unknown";
    }
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
    size_t index;
    int application_written = 0;
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
    if (model->active_application_id[0] != '\0') {
        application_written = snprintf(out_snapshot->active_application_id,
            sizeof(out_snapshot->active_application_id), "%s",
            model->active_application_id);
    }
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
    if (application_written < 0 || first < 0 || second < 0 || third < 0 ||
        (size_t)application_written >=
            sizeof(out_snapshot->active_application_id) ||
        (size_t)first >= sizeof(out_snapshot->active_layout_id) ||
        (size_t)second >= sizeof(out_snapshot->active_layout_name) ||
        (size_t)third >= sizeof(out_snapshot->selected_window_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < model->application_count; ++index) {
        const UmiDesktopTaskbarItem *item = &model->applications[index];
        if (!item->visible) continue;
        out_snapshot->application_count += 1U;
        if (item->pinned) out_snapshot->pinned_application_count += 1U;
        if (item->running) out_snapshot->running_application_count += 1U;
    }
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
