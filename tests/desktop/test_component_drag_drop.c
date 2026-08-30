/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_component_drag_drop.c
 *
 * PURPOSE:
 *   Implement the test component drag drop behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Federated component drag/drop | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application/application.h"
#include "umicom/desktop/component_drag_drop.h"

static UmiStatus create_view(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    return umi_ui_view_model_create(
        view_id, (const char *)user_data, UMI_UI_ROLE_PANE, out_view);
}

static void register_factory(
    UmiDesktopContentRuntime *content,
    const char *view_type)
{
    UmiUiViewFactoryDescriptor factory = {0};
    (void)snprintf(factory.view_type, sizeof(factory.view_type), "%s",
                   view_type);
    (void)snprintf(factory.provider_id, sizeof(factory.provider_id), "%s",
                   "org.umicom.test.drag-drop");
    factory.create = create_view;
    factory.user_data = (void *)view_type;
    assert(umi_ui_view_factory_register(
        umi_desktop_content_runtime_view_factories(content), &factory) ==
        UMI_STATUS_OK);
}

static void bind_window(
    UmiDesktopContentRuntime *content,
    const char *window_id,
    const char *view_id,
    UmiUiWindowGroupRole role)
{
    UmiDesktopContentBinding binding = {0};
    binding.structure_size = (uint32_t)sizeof(binding);
    (void)snprintf(binding.window_id, sizeof(binding.window_id), "%s",
                   window_id);
    (void)snprintf(binding.view_type, sizeof(binding.view_type), "%s",
                   "test.drag-view");
    (void)snprintf(binding.view_id, sizeof(binding.view_id), "%s", view_id);
    binding.context_role = role;
    assert(umi_desktop_content_runtime_bind_window(content, &binding) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiApplicationContextHub *context_hub = NULL;
    UmiFederationRouter *federation = NULL;
    UmiDesktopRuntime *desktop = NULL;
    UmiDesktopContentRuntime *content = NULL;
    UmiDesktopComponentDragDrop *drag_drop = NULL;
    UmiDesktopComponentDropTarget target = {0};
    UmiDesktopComponentDragSnapshot drag;
    UmiDesktopContentSnapshot copied;
    const UmiDesktopWindow *window;

    assert(umi_application_context_hub_create(&context_hub) == UMI_STATUS_OK);
    assert(umi_federation_router_create(&federation) == UMI_STATUS_OK);
    assert(umi_federation_router_register_application(
        federation, umi_application_portfolio_find("org.umicom.studio")) ==
        UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(context_hub, &desktop) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(desktop) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_clone_layout(
        desktop, "develop", "drag-drop-test", "Drag Drop Test", true) ==
        UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_create(
        desktop, federation, &content) == UMI_STATUS_OK);
    register_factory(content, "test.drag-view");
    bind_window(content, "project-explorer", "drag.project", UMI_UI_WINDOW_GROUP_SOURCE);
    bind_window(content, "output", "drag.output", UMI_UI_WINDOW_GROUP_DESTINATION);
    assert(umi_desktop_component_drag_drop_create(
        desktop, content, &drag_drop) == UMI_STATUS_OK);

    assert(umi_desktop_component_drag_begin(
        drag_drop, "drag.move", "project-explorer",
        UMI_DESKTOP_COMPONENT_DRAG_MOVE) == UMI_STATUS_OK);
    target.structure_size = (uint32_t)sizeof(target);
    (void)snprintf(target.monitor_id, sizeof(target.monitor_id), "%s", "primary");
    target.bounds = (UmiDesktopRect){420, 160, 620, 540};
    target.placement = UMI_DESKTOP_DOCK_FLOATING;
    assert(umi_desktop_component_drag_target(
        drag_drop, "drag.move", &target) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_commit(
        drag_drop, "drag.move") == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(desktop), "project-explorer");
    assert(window != NULL && window->bounds.x == 420);

    (void)memset(&target, 0, sizeof(target));
    target.structure_size = (uint32_t)sizeof(target);
    (void)snprintf(target.monitor_id, sizeof(target.monitor_id), "%s", "primary");
    (void)snprintf(target.new_window_id, sizeof(target.new_window_id), "%s",
                   "project-explorer-copy");
    (void)snprintf(target.new_view_id, sizeof(target.new_view_id), "%s",
                   "drag.project.copy");
    (void)snprintf(target.title, sizeof(target.title), "%s",
                   "Project Explorer Copy");
    target.bounds = (UmiDesktopRect){1060, 160, 600, 540};
    target.placement = UMI_DESKTOP_DOCK_FLOATING;
    target.context_role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
    assert(umi_desktop_component_drag_begin(
        drag_drop, "drag.copy", "project-explorer",
        UMI_DESKTOP_COMPONENT_DRAG_COPY) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_target(
        drag_drop, "drag.copy", &target) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_commit(
        drag_drop, "drag.copy") == UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_snapshot(
        content, "project-explorer-copy", &copied) == UMI_STATUS_OK);
    assert(strcmp(copied.window.owner_application_id,
                  "org.umicom.studio") == 0);

    (void)memset(&target, 0, sizeof(target));
    target.structure_size = (uint32_t)sizeof(target);
    (void)snprintf(target.target_window_id,
                   sizeof(target.target_window_id), "%s", "project-explorer");
    target.context_role = UMI_UI_WINDOW_GROUP_DESTINATION;
    assert(umi_desktop_component_drag_begin(
        drag_drop, "drag.link", "output",
        UMI_DESKTOP_COMPONENT_DRAG_LINK) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_target(
        drag_drop, "drag.link", &target) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_commit(
        drag_drop, "drag.link") == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(desktop), "output");
    assert(window != NULL &&
           strcmp(window->context_group_id, "project-blue") == 0);
    assert(umi_desktop_component_drag_snapshot(
        drag_drop, "drag.link", &drag) == UMI_STATUS_OK);
    assert(drag.state == UMI_DESKTOP_COMPONENT_DRAG_COMMITTED);
    assert(umi_desktop_component_drag_count(drag_drop) == 3U);

    assert(umi_desktop_component_drag_begin(
        drag_drop, "drag.cancel", "output",
        UMI_DESKTOP_COMPONENT_DRAG_MOVE) == UMI_STATUS_OK);
    assert(umi_desktop_component_drag_cancel(
        drag_drop, "drag.cancel") == UMI_STATUS_OK);

    umi_desktop_component_drag_drop_destroy(drag_drop);
    umi_desktop_content_runtime_destroy(content);
    umi_desktop_runtime_destroy(desktop);
    umi_federation_router_destroy(federation);
    umi_application_context_hub_destroy(context_hub);
    return 0;
}
