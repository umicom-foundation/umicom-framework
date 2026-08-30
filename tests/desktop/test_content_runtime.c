/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_content_runtime.c
 *
 * PURPOSE:
 *   Implement the test content runtime behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Federated desktop content runtime | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/application.h"
#include "umicom/desktop/content_runtime.h"

static UmiStatus create_content_view(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    UmiUiValue value;
    UmiStatus status;
    const char *view_type = (const char *)user_data;
    status = umi_ui_view_model_create(
        view_id, view_type, UMI_UI_ROLE_PANE, out_view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_value_set_string(&value, "Federated Framework content");
    if (status == UMI_STATUS_OK)
        status = umi_ui_view_model_set_property(*out_view, "summary", &value);
    return status;
}

static UmiStatus handle_ai(
    void *context,
    const UmiFederationRequest *request,
    UmiFederationResponse *response)
{
    int *calls = (int *)context;
    (void)response;
    assert(strcmp(request->operation_id, "chat.inspect") == 0);
    *calls += 1;
    return UMI_STATUS_OK;
}

static void register_factory(
    UmiDesktopContentRuntime *content,
    const char *view_type)
{
    UmiUiViewFactoryDescriptor factory = {0};
    (void)snprintf(factory.view_type, sizeof(factory.view_type), "%s",
                   view_type);
    (void)snprintf(factory.provider_id, sizeof(factory.provider_id), "%s",
                   "org.umicom.test.content");
    factory.create = create_content_view;
    factory.user_data = (void *)view_type;
    assert(umi_ui_view_factory_register(
        umi_desktop_content_runtime_view_factories(content), &factory) ==
        UMI_STATUS_OK);
}

int main(void)
{
    UmiApplicationContextHub *context_hub = NULL;
    UmiFederationRouter *federation = NULL;
    UmiDesktopRuntime *desktop = NULL;
    UmiDesktopContentRuntime *content;
    UmiDesktopContentBinding binding = {0};
    UmiDesktopContentMountRequest mount = {0};
    UmiDesktopContentMessage message = {0};
    UmiDesktopContentSnapshot snapshot;
    UmiApplicationContext context;
    UmiUiViewPresentation *presentation =
        (UmiUiViewPresentation *)calloc(1U, sizeof(*presentation));
    int calls = 0;

    assert(presentation != NULL);
    assert(umi_application_context_hub_create(&context_hub) == UMI_STATUS_OK);
    assert(umi_federation_router_create(&federation) == UMI_STATUS_OK);
    assert(umi_federation_router_register_application(
        federation, umi_application_portfolio_find("org.umicom.studio")) ==
        UMI_STATUS_OK);
    assert(umi_federation_router_register_application(
        federation, umi_application_portfolio_find("org.umicom.llm")) ==
        UMI_STATUS_OK);
    assert(umi_federation_router_register_application(
        federation, umi_application_portfolio_find("org.umicom.trader")) ==
        UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(context_hub, &desktop) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(desktop) == UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_create(
        desktop, federation, &content) == UMI_STATUS_OK);
    register_factory(content, "test.ai-content");
    register_factory(content, "test.trading-content");

    assert(umi_federation_router_register_provider(
        federation, "org.umicom.llm", "umicom.ai", handle_ai, &calls) ==
        UMI_STATUS_OK);

    binding.structure_size = (uint32_t)sizeof(binding);
    (void)snprintf(binding.window_id, sizeof(binding.window_id), "%s",
                   "authorengine");
    (void)snprintf(binding.view_type, sizeof(binding.view_type), "%s",
                   "test.ai-content");
    (void)snprintf(binding.view_id, sizeof(binding.view_id), "%s",
                   "authorengine-content");
    binding.context_role = UMI_UI_WINDOW_GROUP_BIDIRECTIONAL;
    assert(umi_desktop_content_runtime_bind_window(content, &binding) ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_snapshot(
        content, "authorengine", &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.capability_id, "umicom.ai") == 0);
    assert(!snapshot.owns_window);
    assert(umi_desktop_content_runtime_presentation(
        content, "authorengine", presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation->view.view_id, "authorengine-content") == 0);

    message.structure_size = (uint32_t)sizeof(message);
    message.message_id = "content-message-1";
    message.operation_id = "chat.inspect";
    message.schema_id = "umicom.ai.chat.v1";
    message.correlation_id = "content-correlation-1";
    message.kind = UMI_FEDERATION_QUERY;
    assert(umi_desktop_content_runtime_dispatch(
        content, "authorengine", &message, NULL) == UMI_STATUS_OK);
    assert(calls == 1);
    assert(umi_desktop_content_runtime_publish_context(
        content, "authorengine", "project", "umicom-studio", "context-1") ==
        UMI_STATUS_OK);
    assert(umi_application_context_hub_snapshot(
        context_hub, "project-blue", &context) == UMI_STATUS_OK);
    assert(strcmp(context.source_application_id, "org.umicom.llm") == 0);

    assert(umi_desktop_runtime_clone_layout(
        desktop, "develop",
        "content-test", "Content Test", true) == UMI_STATUS_OK);
    mount.structure_size = (uint32_t)sizeof(mount);
    (void)snprintf(mount.window.window_id,
                   sizeof(mount.window.window_id), "%s", "watchlist-test");
    (void)snprintf(mount.window.title, sizeof(mount.window.title), "%s",
                   "Watchlist Test");
    (void)snprintf(mount.window.component_id,
                   sizeof(mount.window.component_id), "%s",
                   "umicom.trading.watchlist");
    (void)snprintf(mount.window.owner_application_id,
                   sizeof(mount.window.owner_application_id), "%s",
                   "org.umicom.trader");
    (void)snprintf(mount.window.monitor_id,
                   sizeof(mount.window.monitor_id), "%s", "primary");
    (void)snprintf(mount.window.context_group_id,
                   sizeof(mount.window.context_group_id), "%s",
                   "trading-teal");
    mount.window.bounds = (UmiDesktopRect){100, 100, 600, 500};
    mount.window.dock_placement = UMI_DESKTOP_DOCK_FLOATING;
    mount.window.visible = true;
    mount.window.closable = true;
    mount.window.resizable = true;
    (void)snprintf(mount.view_type, sizeof(mount.view_type), "%s",
                   "test.trading-content");
    (void)snprintf(mount.view_id, sizeof(mount.view_id), "%s",
                   "watchlist-test-content");
    mount.context_role = UMI_UI_WINDOW_GROUP_SOURCE;
    assert(umi_desktop_content_runtime_mount(content, &mount) == UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_count(content) == 2U);
    assert(umi_desktop_content_runtime_activate(content, "watchlist-test") ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_suspend(content, "watchlist-test") ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_resume(content, "watchlist-test") ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_show(content, "watchlist-test", 0) ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_unmount(content, "watchlist-test") ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_unmount(content, "authorengine") ==
           UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_count(content) == 0U);

    umi_desktop_content_runtime_destroy(content);
    umi_desktop_runtime_destroy(desktop);
    umi_federation_router_destroy(federation);
    umi_application_context_hub_destroy(context_hub);
    free(presentation);
    return 0;
}
