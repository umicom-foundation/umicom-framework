/*-----------------------------------------------------------------------------
 * Umicom Framework | Master-controlled federated desktop runtime
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDesktopRuntime {
    UmiDesktopMonitorTopology monitors;
    UmiDesktopLayoutTabs tabs;
    UmiDesktopLayoutCatalogue layouts;
    UmiDesktopWindowManager windows;
    UmiDesktopContextLinks context_links;
    uint64_t revision;
};

static UmiStatus add_default_monitor(UmiDesktopRuntime *runtime)
{
    UmiDesktopMonitor monitor;
    (void)memset(&monitor, 0, sizeof(monitor));
    (void)snprintf(monitor.monitor_id, sizeof(monitor.monitor_id), "primary");
    (void)snprintf(monitor.name, sizeof(monitor.name), "Primary Monitor");
    monitor.bounds = (UmiDesktopRect){0, 0, 1920, 1080};
    monitor.work_area = (UmiDesktopRect){0, 0, 1920, 1040};
    monitor.scale = 1.0;
    monitor.refresh_rate_hz = 60.0;
    monitor.primary = true;
    monitor.enabled = true;
    return umi_desktop_monitor_topology_add(&runtime->monitors, &monitor);
}

static UmiStatus add_layout_tab(
    UmiDesktopRuntime *runtime,
    const UmiDesktopLayout *layout)
{
    UmiDesktopLayoutTab tab;
    int first;
    int second;
    int third;
    (void)memset(&tab, 0, sizeof(tab));
    first = snprintf(tab.tab_id, sizeof(tab.tab_id), "%s", layout->layout_id);
    second = snprintf(tab.layout_id, sizeof(tab.layout_id), "%s",
                      layout->layout_id);
    third = snprintf(tab.label, sizeof(tab.label), "%s", layout->name);
    if (first < 0 || second < 0 || third < 0 ||
        (size_t)first >= sizeof(tab.tab_id) ||
        (size_t)second >= sizeof(tab.layout_id) ||
        (size_t)third >= sizeof(tab.label))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    tab.active = strcmp(layout->layout_id,
                        runtime->layouts.active_layout_id) == 0;
    tab.pinned = layout->built_in;
    tab.closable = !layout->built_in;
    return umi_desktop_layout_tabs_add(&runtime->tabs, &tab);
}

static UmiStatus add_layout_tabs(UmiDesktopRuntime *runtime)
{
    size_t index;
    for (index = 0U; index < runtime->layouts.count; ++index) {
        const UmiDesktopLayout *layout = &runtime->layouts.layouts[index];
        UmiStatus status = add_layout_tab(runtime, layout);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static UmiStatus define_context_group(
    UmiDesktopRuntime *runtime,
    const char *group_id,
    const char *colour,
    UmiUiWindowContextKind kind)
{
    return umi_desktop_context_links_define(
        &runtime->context_links, group_id, colour, kind);
}

static UmiStatus seed_context_links(UmiDesktopRuntime *runtime)
{
    static const struct ContextGroupDefinition {
        const char *group_id;
        const char *colour;
        UmiUiWindowContextKind kind;
    } definitions[] = {
        {"project-blue", "workspace.group.blue", UMI_UI_WINDOW_CONTEXT_PROJECT},
        {"debug-orange", "workspace.group.debug", UMI_UI_WINDOW_CONTEXT_RUN},
        {"run-green", "workspace.group.green", UMI_UI_WINDOW_CONTEXT_RUN},
        {"trading-teal", "workspace.group.trading", UMI_UI_WINDOW_CONTEXT_ACCOUNT},
        {"treasury-blue", "workspace.group.treasury", UMI_UI_WINDOW_CONTEXT_ACCOUNT},
        {"compare-orange", "workspace.group.orange", UMI_UI_WINDOW_CONTEXT_FILE},
        {"media-violet", "workspace.group.media", UMI_UI_WINDOW_CONTEXT_GENERIC},
        {"music-pink", "workspace.group.music", UMI_UI_WINDOW_CONTEXT_GENERIC},
        {"creator-cyan", "workspace.group.creator", UMI_UI_WINDOW_CONTEXT_GENERIC},
        {"rag-green", "workspace.group.rag", UMI_UI_WINDOW_CONTEXT_PROJECT},
        {"llm-purple", "workspace.group.llm", UMI_UI_WINDOW_CONTEXT_GENERIC},
        {"games-green", "workspace.group.games", UMI_UI_WINDOW_CONTEXT_PROJECT},
        {"cad-blue", "workspace.group.cad", UMI_UI_WINDOW_CONTEXT_PROJECT},
        {"kitchen-amber", "workspace.group.kitchen", UMI_UI_WINDOW_CONTEXT_PROJECT},
        {"author-gold", "workspace.group.author", UMI_UI_WINDOW_CONTEXT_PROJECT}
    };
    size_t definition_index;
    size_t layout_index;
    UmiStatus status;
    for (definition_index = 0U;
         definition_index < sizeof(definitions) / sizeof(definitions[0]);
         ++definition_index) {
        status = define_context_group(runtime,
                                      definitions[definition_index].group_id,
                                      definitions[definition_index].colour,
                                      definitions[definition_index].kind);
        if (status != UMI_STATUS_OK) return status;
    }
    for (layout_index = 0U; layout_index < runtime->layouts.count;
         ++layout_index) {
        const UmiDesktopLayout *layout = &runtime->layouts.layouts[layout_index];
        size_t window_index;
        for (window_index = 0U; window_index < layout->window_count;
             ++window_index) {
            const UmiDesktopWindow *window = &layout->windows[window_index];
            if (window->context_group_id[0] == '\0') continue;
            UmiUiWindowGroupRole role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
            if (strcmp(window->window_id, "focus-editor") == 0 ||
                strcmp(window->window_id, "output") == 0)
                continue;
            if (strcmp(window->window_id, "project-explorer") == 0 ||
                strcmp(window->window_id, "metrics") == 0 ||
                strcmp(window->window_id, "compare-original") == 0 ||
                strcmp(window->window_id, "debug-explorer") == 0 ||
                strcmp(window->window_id, "trading-watchlists") == 0)
                role = UMI_UI_WINDOW_GROUP_SOURCE;
            else if (strcmp(window->context_group_id, "project-blue") == 0 ||
                     strcmp(window->context_group_id, "run-green") == 0 ||
                     strcmp(window->context_group_id, "compare-orange") == 0 ||
                     strcmp(window->context_group_id, "debug-orange") == 0 ||
                     strcmp(window->context_group_id, "trading-teal") == 0)
                role = UMI_UI_WINDOW_GROUP_DESTINATION;
            status = umi_desktop_context_links_join(
                &runtime->context_links, window->context_group_id,
                window->window_id, role);
            if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS)
                return status;
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus load_active_windows(UmiDesktopRuntime *runtime)
{
    const UmiDesktopLayout *layout =
        umi_desktop_layout_catalogue_active(&runtime->layouts);
    size_t index;
    UmiStatus status;
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    umi_desktop_window_manager_init(&runtime->windows);
    for (index = 0U; index < layout->window_count; ++index) {
        status = umi_desktop_window_manager_open(
            &runtime->windows, &runtime->monitors, &layout->windows[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static void update_active_layout_window(
    UmiDesktopRuntime *runtime,
    const UmiDesktopWindow *window)
{
    UmiDesktopLayout *layout =
        umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    size_t index;
    if (layout == NULL || window == NULL || layout->locked) return;
    for (index = 0U; index < layout->window_count; ++index) {
        if (strcmp(layout->windows[index].window_id, window->window_id) == 0) {
            layout->windows[index] = *window;
            layout->revision += 1U;
            return;
        }
    }
}

UmiStatus umi_desktop_runtime_create(
    UmiApplicationContextHub *context_hub,
    UmiDesktopRuntime **out_runtime)
{
    UmiDesktopRuntime *runtime;
    if (context_hub == NULL || out_runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiDesktopRuntime *)calloc(1U, sizeof(*runtime));
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_desktop_monitor_topology_init(&runtime->monitors);
    umi_desktop_layout_tabs_init(&runtime->tabs);
    umi_desktop_layout_catalogue_init(&runtime->layouts);
    umi_desktop_window_manager_init(&runtime->windows);
    umi_desktop_context_links_init(&runtime->context_links, context_hub);
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

void umi_desktop_runtime_destroy(UmiDesktopRuntime *runtime)
{
    free(runtime);
}

UmiStatus umi_desktop_runtime_seed(UmiDesktopRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (runtime->revision != 0U) return UMI_STATUS_INVALID_STATE;
    status = add_default_monitor(runtime);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_catalogue_seed_professional(
            &runtime->layouts, "primary");
    if (status == UMI_STATUS_OK) status = add_layout_tabs(runtime);
    if (status == UMI_STATUS_OK) status = seed_context_links(runtime);
    if (status == UMI_STATUS_OK) status = load_active_windows(runtime);
    if (status == UMI_STATUS_OK) runtime->revision = 1U;
    return status;
}

UmiStatus umi_desktop_runtime_activate_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id)
{
    UmiStatus status;
    if (runtime == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_layout_catalogue_activate(&runtime->layouts, layout_id);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_tabs_activate(&runtime->tabs, layout_id);
    if (status == UMI_STATUS_OK) status = load_active_windows(runtime);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desktop_runtime_clone_layout(
    UmiDesktopRuntime *runtime,
    const char *source_layout_id,
    const char *layout_id,
    const char *name,
    bool activate)
{
    const UmiDesktopLayout *layout;
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_layout_catalogue_clone(
        &runtime->layouts, source_layout_id, layout_id, name);
    if (status != UMI_STATUS_OK) return status;
    layout = umi_desktop_layout_catalogue_find(&runtime->layouts, layout_id);
    status = add_layout_tab(runtime, layout);
    if (status != UMI_STATUS_OK) {
        (void)umi_desktop_layout_catalogue_remove(&runtime->layouts, layout_id);
        return status;
    }
    if (activate) status = umi_desktop_runtime_activate_layout(runtime, layout_id);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desktop_runtime_replace_layout(
    UmiDesktopRuntime *runtime,
    const UmiDesktopLayout *layout,
    bool activate)
{
    size_t index;
    UmiStatus status = UMI_STATUS_NOT_FOUND;
    if (runtime == NULL || layout == NULL || layout->layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < runtime->layouts.count; ++index) {
        UmiDesktopLayout *stored = &runtime->layouts.layouts[index];
        if (strcmp(stored->layout_id, layout->layout_id) != 0) continue;
        if (stored->built_in || stored->locked)
            return UMI_STATUS_PERMISSION_DENIED;
        *stored = *layout;
        stored->built_in = false;
        stored->locked = false;
        stored->revision += 1U;
        runtime->layouts.revision += 1U;
        status = UMI_STATUS_OK;
        break;
    }
    if (status != UMI_STATUS_OK) return status;
    if (activate || strcmp(runtime->layouts.active_layout_id,
                           layout->layout_id) == 0) {
        status = umi_desktop_runtime_activate_layout(runtime,
                                                      layout->layout_id);
    }
    if (status == UMI_STATUS_OK) {
        (void)umi_desktop_layout_tabs_set_dirty(
            &runtime->tabs, layout->layout_id, true);
        runtime->revision += 1U;
    }
    return status;
}

UmiStatus umi_desktop_runtime_remove_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id)
{
    const UmiDesktopLayout *layout;
    UmiStatus status;
    if (runtime == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_find(&runtime->layouts, layout_id);
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    if (layout->built_in || layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(runtime->layouts.active_layout_id, layout_id) == 0)
        return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_layout_catalogue_remove(&runtime->layouts, layout_id);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_tabs_remove(&runtime->tabs, layout_id);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_desktop_runtime_commit_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id)
{
    size_t index;
    UmiStatus status;
    if (runtime == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < runtime->layouts.count; ++index) {
        UmiDesktopLayout *layout = &runtime->layouts.layouts[index];
        if (strcmp(layout->layout_id, layout_id) != 0) continue;
        if (layout->built_in || layout->locked)
            return UMI_STATUS_PERMISSION_DENIED;
        layout->revision += 1U;
        runtime->layouts.revision += 1U;
        status = umi_desktop_layout_tabs_set_dirty(
            &runtime->tabs, layout_id, false);
        if (status == UMI_STATUS_OK) runtime->revision += 1U;
        return status;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_desktop_runtime_open_window(
    UmiDesktopRuntime *runtime,
    const UmiDesktopWindow *window)
{
    UmiDesktopLayout *layout;
    UmiStatus status;
    if (runtime == NULL || window == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    if (layout->window_count >= UMI_DESKTOP_MAX_LAYOUT_WINDOWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_desktop_window_manager_open(
        &runtime->windows, &runtime->monitors, window);
    if (status != UMI_STATUS_OK) return status;
    layout->windows[layout->window_count++] = *window;
    layout->revision += 1U;
    (void)umi_desktop_layout_tabs_set_dirty(
        &runtime->tabs, layout->layout_id, true);
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_runtime_close_window(
    UmiDesktopRuntime *runtime,
    const char *window_id)
{
    UmiDesktopLayout *layout;
    size_t index;
    UmiStatus status;
    if (runtime == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    status = umi_desktop_window_manager_close(&runtime->windows, window_id);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < layout->window_count; ++index) {
        if (strcmp(layout->windows[index].window_id, window_id) == 0) {
            if (index + 1U < layout->window_count) {
                (void)memmove(&layout->windows[index],
                              &layout->windows[index + 1U],
                              (layout->window_count - index - 1U) *
                                  sizeof(layout->windows[0]));
            }
            layout->window_count -= 1U;
            break;
        }
    }
    layout->revision += 1U;
    (void)umi_desktop_layout_tabs_set_dirty(
        &runtime->tabs, layout->layout_id, true);
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_runtime_show_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    bool visible)
{
    UmiDesktopLayout *layout;
    UmiStatus status;
    if (runtime == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_window_manager_show(
        &runtime->windows, window_id, visible);
    if (status == UMI_STATUS_OK) {
        update_active_layout_window(
            runtime, umi_desktop_window_manager_find(&runtime->windows,
                                                       window_id));
        if (!layout->locked) {
            (void)umi_desktop_layout_tabs_set_dirty(
                &runtime->tabs, layout->layout_id, true);
        }
        runtime->revision += 1U;
    }
    return status;
}

UmiStatus umi_desktop_runtime_place_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement)
{
    UmiDesktopLayout *layout;
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    status = umi_desktop_window_manager_place(
        &runtime->windows, &runtime->monitors, window_id, monitor_id,
        bounds, placement);
    if (status == UMI_STATUS_OK) {
        update_active_layout_window(
            runtime, umi_desktop_window_manager_find(&runtime->windows,
                                                       window_id));
        (void)umi_desktop_layout_tabs_set_dirty(
            &runtime->tabs, layout->layout_id, true);
        runtime->revision += 1U;
    }
    return status;
}

UmiStatus umi_desktop_runtime_snapshot(
    const UmiDesktopRuntime *runtime,
    UmiDesktopSnapshot *out_snapshot)
{
    const UmiDesktopMonitor *primary;
    int first;
    int second;
    if (runtime == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->monitor_count = runtime->monitors.count;
    out_snapshot->layout_count = runtime->layouts.count;
    out_snapshot->tab_count = runtime->tabs.count;
    out_snapshot->window_count = runtime->windows.count;
    out_snapshot->context_group_count = runtime->context_links.groups.count;
    out_snapshot->tab_placement = runtime->tabs.placement;
    first = snprintf(out_snapshot->active_layout_id,
                     sizeof(out_snapshot->active_layout_id), "%s",
                     runtime->layouts.active_layout_id);
    primary = umi_desktop_monitor_topology_primary(&runtime->monitors);
    second = primary != NULL
        ? snprintf(out_snapshot->primary_monitor_id,
                   sizeof(out_snapshot->primary_monitor_id), "%s",
                   primary->monitor_id) : 0;
    if (first < 0 || second < 0 ||
        (size_t)first >= sizeof(out_snapshot->active_layout_id) ||
        (size_t)second >= sizeof(out_snapshot->primary_monitor_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_snapshot->revision = runtime->revision + runtime->monitors.revision +
        runtime->tabs.revision + runtime->layouts.revision +
        runtime->windows.revision + runtime->context_links.revision;
    return UMI_STATUS_OK;
}

UmiDesktopMonitorTopology *umi_desktop_runtime_monitors(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->monitors : NULL;
}

UmiDesktopLayoutTabs *umi_desktop_runtime_tabs(UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->tabs : NULL;
}

UmiDesktopLayoutCatalogue *umi_desktop_runtime_layouts(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->layouts : NULL;
}

UmiDesktopWindowManager *umi_desktop_runtime_windows(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->windows : NULL;
}

UmiDesktopContextLinks *umi_desktop_runtime_context_links(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->context_links : NULL;
}
