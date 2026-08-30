/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/ui_bridge.c
 *
 * PURPOSE:
 *   Implement the ui bridge behavior for
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
 * Umicom Framework | Desktop to semantic UI bridge
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/ui_bridge.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/component_catalogue.h"

static const struct ToolMapping {
    const char *component_id;
    const char *tool_id;
} TOOL_MAPPINGS[] = {
    {"umicom.development.explorer", "project-explorer"},
    {"umicom.development.editor", "editor"},
    {"umicom.development.build", "build"},
    {"umicom.development.testing", "tests"},
    {"umicom.development.debug", "debug"},
    {"umicom.development.source-control", "source-control"},
    {"umicom.development.terminal", "terminal"},
    {"umicom.ai.chat", "authorengine"},
    {"umicom.trading.watchlist", "watchlist"},
    {"umicom.trading.chart", "chart"},
    {"umicom.trading.order-ticket", "order-entry"},
    {"umicom.trading.portfolio", "portfolio"},
    {"umicom.trading.orders", "trading-activity"},
    {"umicom.database.explorer", "database-explorer"},
    {"umicom.operations.metrics", "metrics"},
    {"umicom.operations.logs", "traces"},
    {"umicom.operations.health", "health"}
};

static const char *mapped_component_tool_id(const char *component_id)
{
    size_t index;
    if (component_id == NULL) return NULL;
    for (index = 0U; index < sizeof(TOOL_MAPPINGS) / sizeof(TOOL_MAPPINGS[0]);
         ++index) {
        if (strcmp(component_id, TOOL_MAPPINGS[index].component_id) == 0)
            return TOOL_MAPPINGS[index].tool_id;
    }
    return component_id;
}

static const char *presentation_tool_id(const UmiDesktopWindow *window)
{
    const char *component_id;
    if (window == NULL) return "unknown";
    if (strcmp(window->window_id, "debug-console") == 0)
        return "debug-console";
    if (strcmp(window->window_id, "compare-original") == 0 ||
        strcmp(window->window_id, "compare-modified") == 0)
        return "file-compare";
    if (strcmp(window->window_id, "compare-results") == 0)
        return "output";
    if (strcmp(window->window_id, "traces") == 0) return "traces";
    if (strcmp(window->window_id, "profiler") == 0) return "profiler";
    if (strcmp(window->window_id, "health") == 0) return "health";
    if (strcmp(window->window_id, "debug-state") == 0)
        return "debug-workspace";
    if (strcmp(window->window_id, "output") == 0) return "output";
    component_id = window->component_id;
    return mapped_component_tool_id(component_id);
}

static UmiStatus project_window(
    const UmiDesktopWindow *source,
    const UmiDesktopMonitorTopology *topology,
    UmiUiWorkspaceWindow *out_window)
{
    const UmiDesktopMonitor *monitor;
    int first;
    int second;
    int third;
    int fourth = 0;
    if (source == NULL || topology == NULL || out_window == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    monitor = umi_desktop_monitor_topology_find(topology, source->monitor_id);
    if (monitor == NULL || monitor->work_area.width <= 0 ||
        monitor->work_area.height <= 0) return UMI_STATUS_NOT_FOUND;
    (void)memset(out_window, 0, sizeof(*out_window));
    first = snprintf(out_window->window_id, sizeof(out_window->window_id),
                     "%s", source->window_id);
    second = snprintf(out_window->title, sizeof(out_window->title), "%s",
                      source->title);
    third = snprintf(out_window->tool_id, sizeof(out_window->tool_id), "%s",
                     presentation_tool_id(source));
    if (source->context_group_id[0] != '\0')
        fourth = snprintf(out_window->group_id, sizeof(out_window->group_id),
                          "%s", source->context_group_id);
    if (first < 0 || second < 0 || third < 0 || fourth < 0 ||
        (size_t)first >= sizeof(out_window->window_id) ||
        (size_t)second >= sizeof(out_window->title) ||
        (size_t)third >= sizeof(out_window->tool_id) ||
        (size_t)fourth >= sizeof(out_window->group_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_window->x = (double)(source->bounds.x - monitor->work_area.x) /
        (double)monitor->work_area.width;
    out_window->y = (double)(source->bounds.y - monitor->work_area.y) /
        (double)monitor->work_area.height;
    out_window->width = (double)source->bounds.width /
        (double)monitor->work_area.width;
    out_window->height = (double)source->bounds.height /
        (double)monitor->work_area.height;
    out_window->visible = source->visible;
    out_window->floating =
        source->dock_placement == UMI_DESKTOP_DOCK_FLOATING;
    out_window->maximised = source->maximised;
    out_window->closable = source->closable;
    out_window->z_order = source->z_order;
    return UMI_STATUS_OK;
}

static UmiStatus project_layout(
    const UmiDesktopLayout *source,
    const UmiDesktopMonitorTopology *topology,
    UmiUiWorkspaceLayout *out_layout)
{
    char preset_layout_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    int length;
    size_t index;
    UmiStatus status;
    length = snprintf(preset_layout_id, sizeof(preset_layout_id), "preset-%s",
                      source->layout_id);
    if (length < 0 || (size_t)length >= sizeof(preset_layout_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_ui_workspace_layout_init(
        out_layout, preset_layout_id, source->name);
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_layout_set_locked(out_layout, false);
    for (index = 0U; status == UMI_STATUS_OK && index < source->window_count;
         ++index) {
        UmiUiWorkspaceWindow window;
        status = project_window(&source->windows[index], topology, &window);
        if (status == UMI_STATUS_OK)
            status = umi_ui_workspace_layout_add_window(out_layout, &window);
    }
    if (status == UMI_STATUS_OK)
        status = umi_ui_workspace_layout_set_locked(out_layout, source->locked);
    return status;
}

UmiStatus umi_desktop_seed_workspace_customisation(
    UmiUiWorkspaceCustomisation *customisation,
    const UmiDesktopLayoutCatalogue *catalogue,
    const UmiDesktopMonitorTopology *topology,
    const char *preset_namespace)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;
    if (customisation == NULL || catalogue == NULL || topology == NULL ||
        preset_namespace == NULL || preset_namespace[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->count > UMI_UI_CUSTOM_WORKSPACE_MAX_LAYOUTS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiDesktopLayout *source = &catalogue->layouts[index];
        UmiUiLayoutLibraryItem item;
        UmiUiWorkspaceLayout instance;
        int first;
        int second;
        int third;
        (void)memset(&item, 0, sizeof(item));
        first = snprintf(item.preset_id, sizeof(item.preset_id), "%s-%s",
                         preset_namespace, source->layout_id);
        second = snprintf(item.category, sizeof(item.category), "%s",
                          source->category);
        third = snprintf(item.description, sizeof(item.description), "%s",
                         source->description);
        if (first < 0 || second < 0 || third < 0 ||
            (size_t)first >= sizeof(item.preset_id) ||
            (size_t)second >= sizeof(item.category) ||
            (size_t)third >= sizeof(item.description))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        status = project_layout(source, topology, &item.layout);
        if (status == UMI_STATUS_OK)
            status = umi_ui_layout_library_add(&customisation->library, &item);
        if (status == UMI_STATUS_OK)
            status = umi_ui_layout_library_instantiate(
                &customisation->library, item.preset_id, source->layout_id,
                source->name, &instance);
        if (status == UMI_STATUS_OK)
            status = umi_ui_workspace_customisation_add_layout(
                customisation, &instance);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_ui_workspace_customisation_activate(
        customisation, catalogue->active_layout_id);
}

UmiStatus umi_desktop_seed_workspace_groups(
    UmiUiWindowGroupStore *groups,
    const UmiDesktopContextLinks *context_links)
{
    size_t group_index;
    if (groups == NULL || context_links == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (group_index = 0U; group_index < context_links->groups.count;
         ++group_index) {
        const UmiUiWindowGroup *source =
            &context_links->groups.items[group_index];
        size_t member_index;
        UmiStatus status = umi_ui_window_group_define(
            groups, source->group_id, source->colour_token,
            source->context_kind);
        if (status != UMI_STATUS_OK) return status;
        for (member_index = 0U; member_index < source->member_count;
             ++member_index) {
            status = umi_ui_window_group_join(
                groups, source->group_id,
                source->members[member_index].window_id,
                source->members[member_index].role);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

static UmiUiWindowCategory component_category(const char *domain_id)
{
    if (strcmp(domain_id, "development") == 0)
        return UMI_UI_WINDOW_CATEGORY_DEVELOPMENT;
    if (strcmp(domain_id, "shell") == 0)
        return UMI_UI_WINDOW_CATEGORY_NAVIGATION;
    if (strcmp(domain_id, "operations") == 0 ||
        strcmp(domain_id, "security") == 0)
        return UMI_UI_WINDOW_CATEGORY_OPERATIONS;
    if (strcmp(domain_id, "database") == 0 ||
        strcmp(domain_id, "integration") == 0 ||
        strcmp(domain_id, "treasury") == 0)
        return UMI_UI_WINDOW_CATEGORY_DATA;
    if (strcmp(domain_id, "ai") == 0 || strcmp(domain_id, "rag") == 0 ||
        strcmp(domain_id, "llm") == 0 || strcmp(domain_id, "author") == 0)
        return UMI_UI_WINDOW_CATEGORY_AI;
    if (strcmp(domain_id, "trading") == 0)
        return UMI_UI_WINDOW_CATEGORY_TRADING;
    return UMI_UI_WINDOW_CATEGORY_GENERAL;
}

static void component_dimensions(
    UmiApplicationComponentRole role,
    double *width,
    double *height)
{
    switch (role) {
        case UMI_APPLICATION_COMPONENT_EDITOR:
            *width = 0.60; *height = 0.75; break;
        case UMI_APPLICATION_COMPONENT_DASHBOARD:
            *width = 0.50; *height = 0.50; break;
        case UMI_APPLICATION_COMPONENT_INSPECTOR:
            *width = 0.30; *height = 0.60; break;
        case UMI_APPLICATION_COMPONENT_TOOL:
            *width = 0.30; *height = 0.50; break;
        case UMI_APPLICATION_COMPONENT_SERVICE_SURFACE:
            *width = 1.00; *height = 1.00; break;
        case UMI_APPLICATION_COMPONENT_VIEW:
        default:
            *width = 0.35; *height = 0.60; break;
    }
}

UmiStatus umi_desktop_seed_window_catalogue(
    UmiUiWindowCatalogue *catalogue)
{
    static const UmiUiWindowDescriptor legacy_windows[] = {
        {"project-explorer","Project Explorer","Browse workspace projects and files","folder",UMI_UI_WINDOW_CATEGORY_NAVIGATION,false,0.20,0.75},
        {"editor","Editor","Tabbed source editor","text-editor",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.55,0.75},
        {"file-compare","File Compare","Side-by-side file and directory comparison","compare",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.41,0.80},
        {"terminal","Terminal","Shell, task and process terminal","terminal",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.50,0.25},
        {"build","Build","Configure, compile, package and deploy","build",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.50,0.25},
        {"tests","Test Explorer","Discover and execute tests","test",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.35,0.50},
        {"debug","Debug","Breakpoints, stack, variables and watches","debug",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.40,0.50},
        {"source-control","Source Control","Changes, history and branches","source-control",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.30,0.60},
        {"output","Output","Build, task, extension and run output","output",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,1.00,0.25},
        {"metrics","Metrics","Runtime counters and gauges","metrics",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
        {"traces","Traces","Correlated operation traces","traces",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
        {"profiler","Profiler","CPU, memory, process and task profiles","speedometer",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
        {"health","Health and Resilience","Readiness, recovery, retry and circuit evidence","health",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
        {"database-explorer","Database Explorer","Connections, queries, data and migrations","database",UMI_UI_WINDOW_CATEGORY_DATA,false,0.40,0.70},
        {"authorengine","AI and AuthorEngine","Context-aware assistant, agents and generation","sparkles",UMI_UI_WINDOW_CATEGORY_AI,false,0.25,0.75},
        {"chart","Chart","Reusable financial and operational chart","chart",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.50,0.50},
        {"watchlist","Watchlist","Linked instruments and alerts","watchlist",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.30,0.60},
        {"order-entry","Order Entry","Linked trading order ticket","order",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.30,0.50},
        {"portfolio","Portfolio and Risk","Positions, balances, performance and risk","portfolio",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.30,0.50},
        {"trading-activity","Trading Activity","Orders, executions and operational messages","activity",UMI_UI_WINDOW_CATEGORY_TRADING,true,1.00,0.28}
    };
    size_t index;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < sizeof(legacy_windows) / sizeof(legacy_windows[0]);
         ++index) {
        UmiStatus status = umi_ui_window_catalogue_register(
            catalogue, &legacy_windows[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < umi_application_component_catalogue_count();
         ++index) {
        const UmiApplicationComponentDefinition *component =
            umi_application_component_catalogue_at(index);
        const char *tool_id = mapped_component_tool_id(component->component_id);
        UmiUiWindowDescriptor descriptor;
        int first;
        int second;
        int third;
        int fourth;
        UmiStatus status;
        if (umi_ui_window_catalogue_find(catalogue, tool_id) != NULL) continue;
        (void)memset(&descriptor, 0, sizeof(descriptor));
        first = snprintf(descriptor.tool_id, sizeof(descriptor.tool_id), "%s",
                         tool_id);
        second = snprintf(descriptor.title, sizeof(descriptor.title), "%s",
                          component->title);
        third = snprintf(descriptor.description, sizeof(descriptor.description),
                         "%s", component->description);
        fourth = snprintf(descriptor.icon_name, sizeof(descriptor.icon_name),
                          "view-grid-symbolic");
        if (first < 0 || second < 0 || third < 0 || fourth < 0 ||
            (size_t)first >= sizeof(descriptor.tool_id) ||
            (size_t)second >= sizeof(descriptor.title) ||
            (size_t)third >= sizeof(descriptor.description) ||
            (size_t)fourth >= sizeof(descriptor.icon_name))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        descriptor.category = component_category(component->domain_id);
        descriptor.supports_multiple = component->multi_instance != 0;
        component_dimensions(component->role, &descriptor.default_width,
                             &descriptor.default_height);
        status = umi_ui_window_catalogue_register(catalogue, &descriptor);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
