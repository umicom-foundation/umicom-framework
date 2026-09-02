/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/runtime.c
 *
 * PURPOSE:
 *   Implement the runtime behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework | Master-controlled federated desktop runtime
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Provide the add default monitor operation used by this module and its client
 * applications.
 */
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

/* Provide the add layout tab operation used by this module and its client applications. */
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
    /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the add layout tabs operation used by this module and its client applications. */
static UmiStatus add_layout_tabs(UmiDesktopRuntime *runtime)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->layouts.count; ++index) {
        const UmiDesktopLayout *layout = &runtime->layouts.layouts[index];
        UmiStatus status = add_layout_tab(runtime, layout);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the define context group operation used by this module and its client
 * applications.
 */
static UmiStatus define_context_group(
    UmiDesktopRuntime *runtime,
    const char *group_id,
    const char *colour,
    UmiUiWindowContextKind kind)
{
    return umi_desktop_context_links_define(
        &runtime->context_links, group_id, colour, kind);
}

/*
 * Provide the seed context links operation used by this module and its client
 * applications.
 */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (definition_index = 0U;
         definition_index < sizeof(definitions) / sizeof(definitions[0]);
         ++definition_index) {
        status = define_context_group(runtime,
                                      definitions[definition_index].group_id,
                                      definitions[definition_index].colour,
                                      definitions[definition_index].kind);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (layout_index = 0U; layout_index < runtime->layouts.count;
         ++layout_index) {
        const UmiDesktopLayout *layout = &runtime->layouts.layouts[layout_index];
        size_t window_index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (window_index = 0U; window_index < layout->window_count;
             ++window_index) {
            const UmiDesktopWindow *window = &layout->windows[window_index];
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (window->context_group_id[0] == '\0') continue;
            UmiUiWindowGroupRole role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(window->window_id, "focus-editor") == 0 ||
                strcmp(window->window_id, "output") == 0)
                continue;
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(window->window_id, "project-explorer") == 0 ||
                strcmp(window->window_id, "metrics") == 0 ||
                strcmp(window->window_id, "compare-original") == 0 ||
                strcmp(window->window_id, "debug-explorer") == 0 ||
                strcmp(window->window_id, "trading-watchlists") == 0)
                role = UMI_UI_WINDOW_GROUP_SOURCE;
            else /* Use the stable identifier comparison to choose the matching record or policy. */ if (strcmp(window->context_group_id, "project-blue") == 0 ||
                     strcmp(window->context_group_id, "run-green") == 0 ||
                     strcmp(window->context_group_id, "compare-orange") == 0 ||
                     strcmp(window->context_group_id, "debug-orange") == 0 ||
                     strcmp(window->context_group_id, "trading-teal") == 0)
                role = UMI_UI_WINDOW_GROUP_DESTINATION;
            status = umi_desktop_context_links_join(
                &runtime->context_links, window->context_group_id,
                window->window_id, role);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS)
                return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the load active windows operation used by this module and its client
 * applications.
 */
static UmiStatus load_active_windows(UmiDesktopRuntime *runtime)
{
    const UmiDesktopLayout *layout =
        umi_desktop_layout_catalogue_active(&runtime->layouts);
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    umi_desktop_window_manager_init(&runtime->windows);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->window_count; ++index) {
        status = umi_desktop_window_manager_open(
            &runtime->windows, &runtime->monitors, &layout->windows[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the update active layout window operation used by this module and its client
 * applications.
 */
static void update_active_layout_window(
    UmiDesktopRuntime *runtime,
    const UmiDesktopWindow *window)
{
    UmiDesktopLayout *layout =
        umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL || window == NULL || layout->locked) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->window_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(layout->windows[index].window_id, window->window_id) == 0) {
            layout->windows[index] = *window;
            layout->revision += 1U;
            return;
        }
    }
}

/*
 * Initialise desktop runtime from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_desktop_runtime_create(
    UmiApplicationContextHub *context_hub,
    UmiDesktopRuntime **out_runtime)
{
    UmiDesktopRuntime *runtime;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context_hub == NULL || out_runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiDesktopRuntime *)calloc(1U, sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_desktop_monitor_topology_init(&runtime->monitors);
    umi_desktop_layout_tabs_init(&runtime->tabs);
    umi_desktop_layout_catalogue_init(&runtime->layouts);
    umi_desktop_window_manager_init(&runtime->windows);
    umi_desktop_context_links_init(&runtime->context_links, context_hub);
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

/* Release or reset state held by desktop runtime so the same storage can be reused safely. */
void umi_desktop_runtime_destroy(UmiDesktopRuntime *runtime)
{
    free(runtime);
}

/*
 * Provide the desktop runtime seed operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_seed(UmiDesktopRuntime *runtime)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (runtime->revision != 0U) return UMI_STATUS_INVALID_STATE;
    status = add_default_monitor(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_catalogue_seed_professional(
            &runtime->layouts, "primary");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = add_layout_tabs(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = seed_context_links(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = load_active_windows(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision = 1U;
    return status;
}

/*
 * Provide the desktop runtime activate layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_activate_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_layout_catalogue_activate(&runtime->layouts, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_tabs_activate(&runtime->tabs, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = load_active_windows(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop runtime clone layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_clone_layout(
    UmiDesktopRuntime *runtime,
    const char *source_layout_id,
    const char *layout_id,
    const char *name,
    bool activate)
{
    const UmiDesktopLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_layout_catalogue_clone(
        &runtime->layouts, source_layout_id, layout_id, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    layout = umi_desktop_layout_catalogue_find(&runtime->layouts, layout_id);
    status = add_layout_tab(runtime, layout);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_desktop_layout_catalogue_remove(&runtime->layouts, layout_id);
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (activate) status = umi_desktop_runtime_activate_layout(runtime, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop runtime replace layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_replace_layout(
    UmiDesktopRuntime *runtime,
    const UmiDesktopLayout *layout,
    bool activate)
{
    size_t index;
    UmiStatus status = UMI_STATUS_NOT_FOUND;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || layout == NULL || layout->layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->layouts.count; ++index) {
        UmiDesktopLayout *stored = &runtime->layouts.layouts[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(stored->layout_id, layout->layout_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (activate || strcmp(runtime->layouts.active_layout_id,
                           layout->layout_id) == 0) {
        status = umi_desktop_runtime_activate_layout(runtime,
                                                      layout->layout_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_desktop_layout_tabs_set_dirty(
            &runtime->tabs, layout->layout_id, true);
        runtime->revision += 1U;
    }
    return status;
}

/*
 * Provide the desktop runtime remove layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_remove_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id)
{
    const UmiDesktopLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_find(&runtime->layouts, layout_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->built_in || layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(runtime->layouts.active_layout_id, layout_id) == 0)
        return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_layout_catalogue_remove(&runtime->layouts, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_layout_tabs_remove(&runtime->tabs, layout_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop runtime commit layout operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_commit_layout(
    UmiDesktopRuntime *runtime,
    const char *layout_id)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || layout_id == NULL || layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->layouts.count; ++index) {
        UmiDesktopLayout *layout = &runtime->layouts.layouts[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(layout->layout_id, layout_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (layout->built_in || layout->locked)
            return UMI_STATUS_PERMISSION_DENIED;
        layout->revision += 1U;
        runtime->layouts.revision += 1U;
        status = umi_desktop_layout_tabs_set_dirty(
            &runtime->tabs, layout_id, false);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) runtime->revision += 1U;
        return status;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the desktop runtime open window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_open_window(
    UmiDesktopRuntime *runtime,
    const UmiDesktopWindow *window)
{
    UmiDesktopLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (layout->window_count >= UMI_DESKTOP_MAX_LAYOUT_WINDOWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_desktop_window_manager_open(
        &runtime->windows, &runtime->monitors, window);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    layout->windows[layout->window_count++] = *window;
    layout->revision += 1U;
    (void)umi_desktop_layout_tabs_set_dirty(
        &runtime->tabs, layout->layout_id, true);
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop runtime close window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_close_window(
    UmiDesktopRuntime *runtime,
    const char *window_id)
{
    UmiDesktopLayout *layout;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    status = umi_desktop_window_manager_close(&runtime->windows, window_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < layout->window_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(layout->windows[index].window_id, window_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the desktop runtime show window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_show_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    bool visible)
{
    UmiDesktopLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL || window_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_window_manager_show(
        &runtime->windows, window_id, visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        update_active_layout_window(
            runtime, umi_desktop_window_manager_find(&runtime->windows,
                                                       window_id));
        /* Apply this branch only when its contract condition is satisfied. */
        if (!layout->locked) {
            (void)umi_desktop_layout_tabs_set_dirty(
                &runtime->tabs, layout->layout_id, true);
        }
        runtime->revision += 1U;
    }
    return status;
}

/*
 * Provide the desktop runtime place window operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_place_window(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement)
{
    UmiDesktopLayout *layout;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    status = umi_desktop_window_manager_place(
        &runtime->windows, &runtime->monitors, window_id, monitor_id,
        bounds, placement);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the desktop runtime set window context group operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_runtime_set_window_context_group(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *context_group_id)
{
    UmiDesktopLayout *layout;
    UmiDesktopWindow *window;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || window_id == NULL || window_id[0] == '\0' ||
        context_group_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active_mutable(&runtime->layouts);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (layout->locked) return UMI_STATUS_PERMISSION_DENIED;
    window = umi_desktop_window_manager_find_mutable(
        &runtime->windows, window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (window == NULL) return UMI_STATUS_NOT_FOUND;
    length = strlen(context_group_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(window->context_group_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(window->context_group_id, context_group_id, length + 1U);
    update_active_layout_window(runtime, window);
    runtime->layouts.revision += 1U;
    (void)umi_desktop_layout_tabs_set_dirty(
        &runtime->tabs, layout->layout_id, true);
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop runtime restore window session operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_runtime_restore_window_session(
    UmiDesktopRuntime *runtime,
    const char *window_id,
    const char *monitor_id,
    UmiDesktopRect bounds,
    UmiDesktopDockPlacement placement,
    bool visible,
    bool maximised)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_desktop_window_manager_place(
        &runtime->windows, &runtime->monitors, window_id, monitor_id,
        bounds, placement);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_window_manager_show(
            &runtime->windows, window_id, visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_desktop_window_manager_maximise(
            &runtime->windows, window_id, maximised);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Provide the desktop runtime capture state operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_capture_state(
    const UmiDesktopRuntime *runtime,
    UmiDesktopRuntimeState *out_state)
{
    const UmiDesktopLayout *layout;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || out_state == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    layout = umi_desktop_layout_catalogue_active(&runtime->layouts);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (layout == NULL) return UMI_STATUS_INVALID_STATE;
    (void)memset(out_state, 0, sizeof(*out_state));
    out_state->structure_size = (uint32_t)sizeof(*out_state);
    out_state->monitors = runtime->monitors;
    out_state->tabs = runtime->tabs;
    out_state->windows = runtime->windows;
    out_state->context_links = runtime->context_links;
    out_state->active_layout = *layout;
    out_state->revision = runtime->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the validate runtime state operation used by this module and its client
 * applications.
 */
static UmiStatus validate_runtime_state(
    const UmiDesktopRuntime *runtime,
    const UmiDesktopRuntimeState *state)
{
    const UmiDesktopMonitor *primary;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || state == NULL ||
        state->structure_size < sizeof(*state) ||
        state->monitors.count == 0U ||
        state->monitors.count > UMI_DESKTOP_MAX_MONITORS ||
        state->tabs.count > UMI_DESKTOP_MAX_TABS ||
        state->windows.count > UMI_DESKTOP_MAX_WINDOWS ||
        state->context_links.groups.count > UMI_UI_WINDOW_GROUP_MAX ||
        state->active_layout.layout_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    primary = umi_desktop_monitor_topology_primary(&state->monitors);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (primary == NULL || !primary->enabled ||
        umi_desktop_layout_catalogue_find(
            &runtime->layouts, state->active_layout.layout_id) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < state->windows.count; ++index) {
        const UmiDesktopMonitor *monitor =
            umi_desktop_monitor_topology_find(
                &state->monitors,
                state->windows.windows[index].monitor_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (monitor == NULL || !monitor->enabled)
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop runtime restore state operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_restore_state(
    UmiDesktopRuntime *runtime,
    const UmiDesktopRuntimeState *state)
{
    UmiApplicationContextHub *context_hub;
    size_t index;
    UmiStatus status = validate_runtime_state(runtime, state);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    context_hub = runtime->context_links.hub;
    runtime->monitors = state->monitors;
    runtime->tabs = state->tabs;
    runtime->windows = state->windows;
    runtime->context_links = state->context_links;
    runtime->context_links.hub = context_hub;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < runtime->layouts.count; ++index) {
        UmiDesktopLayout *stored = &runtime->layouts.layouts[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(stored->layout_id,
                   state->active_layout.layout_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!stored->built_in && !stored->locked) {
            *stored = state->active_layout;
            stored->built_in = false;
            stored->locked = false;
            stored->revision += 1U;
        }
        break;
    }
    (void)snprintf(runtime->layouts.active_layout_id,
                   sizeof(runtime->layouts.active_layout_id), "%s",
                   state->active_layout.layout_id);
    runtime->layouts.revision += 1U;
    runtime->revision = state->revision >= runtime->revision
        ? state->revision + 1U : runtime->revision + 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_desktop_runtime_snapshot(
    const UmiDesktopRuntime *runtime,
    UmiDesktopSnapshot *out_snapshot)
{
    const UmiDesktopMonitor *primary;
    int first;
    int second;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (first < 0 || second < 0 ||
        (size_t)first >= sizeof(out_snapshot->active_layout_id) ||
        (size_t)second >= sizeof(out_snapshot->primary_monitor_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_snapshot->revision = runtime->revision + runtime->monitors.revision +
        runtime->tabs.revision + runtime->layouts.revision +
        runtime->windows.revision + runtime->context_links.revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop runtime monitors operation used by this module and its client
 * applications.
 */
UmiDesktopMonitorTopology *umi_desktop_runtime_monitors(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->monitors : NULL;
}

/*
 * Provide the desktop runtime tabs operation used by this module and its client
 * applications.
 */
UmiDesktopLayoutTabs *umi_desktop_runtime_tabs(UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->tabs : NULL;
}

/*
 * Provide the desktop runtime layouts operation used by this module and its client
 * applications.
 */
UmiDesktopLayoutCatalogue *umi_desktop_runtime_layouts(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->layouts : NULL;
}

/*
 * Provide the desktop runtime windows operation used by this module and its client
 * applications.
 */
UmiDesktopWindowManager *umi_desktop_runtime_windows(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->windows : NULL;
}

/*
 * Provide the desktop runtime context links operation used by this module and its client
 * applications.
 */
UmiDesktopContextLinks *umi_desktop_runtime_context_links(
    UmiDesktopRuntime *runtime)
{
    return runtime != NULL ? &runtime->context_links : NULL;
}
