/* Umicom Framework Tests | Cross-application context synchronizer | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/application/application.h"
#include "umicom/desktop/context_synchronizer.h"

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
    (void)snprintf(factory.view_type, sizeof(factory.view_type), "%s", view_type);
    (void)snprintf(factory.provider_id, sizeof(factory.provider_id), "%s",
                   "org.umicom.test.context-sync");
    factory.create = create_view;
    factory.user_data = (void *)view_type;
    assert(umi_ui_view_factory_register(
        umi_desktop_content_runtime_view_factories(content), &factory) ==
        UMI_STATUS_OK);
}

static void bind_window(
    UmiDesktopContentRuntime *content,
    const char *window_id,
    const char *view_type,
    UmiUiWindowGroupRole role)
{
    UmiDesktopContentBinding binding = {0};
    binding.structure_size = (uint32_t)sizeof(binding);
    (void)snprintf(binding.window_id, sizeof(binding.window_id), "%s", window_id);
    (void)snprintf(binding.view_type, sizeof(binding.view_type), "%s", view_type);
    (void)snprintf(binding.view_id, sizeof(binding.view_id), "%s.content", window_id);
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
    UmiDesktopContextSynchronizer *synchronizer = NULL;
    UmiDesktopContextSubscription subscription = {0};
    UmiDesktopContextDelivery delivery;
    UmiDesktopContextSynchronizerSnapshot snapshot;

    assert(umi_application_context_hub_create(&context_hub) == UMI_STATUS_OK);
    assert(umi_federation_router_create(&federation) == UMI_STATUS_OK);
    assert(umi_federation_router_register_application(
        federation, umi_application_portfolio_find("org.umicom.studio")) ==
        UMI_STATUS_OK);
    assert(umi_federation_router_register_application(
        federation, umi_application_portfolio_find("org.umicom.llm")) ==
        UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(context_hub, &desktop) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(desktop) == UMI_STATUS_OK);
    assert(umi_desktop_content_runtime_create(
        desktop, federation, &content) == UMI_STATUS_OK);
    register_factory(content, "test.context-source");
    register_factory(content, "test.context-destination");
    bind_window(content, "project-explorer", "test.context-source",
                UMI_UI_WINDOW_GROUP_SOURCE);
    bind_window(content, "authorengine", "test.context-destination",
                UMI_UI_WINDOW_GROUP_DESTINATION);
    assert(umi_desktop_context_synchronizer_create(
        desktop, content, &synchronizer) == UMI_STATUS_OK);

    subscription.structure_size = (uint32_t)sizeof(subscription);
    (void)snprintf(subscription.window_id, sizeof(subscription.window_id), "%s",
                   "authorengine");
    (void)snprintf(subscription.context_type,
                   sizeof(subscription.context_type), "%s", "project");
    subscription.refresh_on_delivery = 1;
    assert(umi_desktop_context_synchronizer_subscribe(
        synchronizer, &subscription) == UMI_STATUS_OK);
    assert(umi_desktop_context_synchronizer_publish(
        synchronizer, "project-explorer", "project", "umicom-studio",
        "context-sync-1") == UMI_STATUS_OK);
    assert(umi_desktop_context_synchronizer_next(
        synchronizer, "authorengine", &delivery) == UMI_STATUS_OK);
    assert(strcmp(delivery.source_application_id, "org.umicom.studio") == 0);
    assert(strcmp(delivery.destination_application_id, "org.umicom.llm") == 0);
    assert(strcmp(delivery.subject_id, "umicom-studio") == 0);
    assert(delivery.refresh_status == UMI_STATUS_OK);
    assert(!delivery.acknowledged);
    assert(umi_desktop_context_synchronizer_acknowledge(
        synchronizer, "authorengine", delivery.sequence) == UMI_STATUS_OK);
    assert(umi_desktop_context_synchronizer_next(
        synchronizer, "authorengine", &delivery) == UMI_STATUS_NOT_FOUND);
    assert(umi_desktop_context_synchronizer_snapshot(
        synchronizer, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.subscription_count == 1U);
    assert(snapshot.delivery_count == 1U);
    assert(snapshot.pending_count == 0U);
    assert(umi_desktop_context_synchronizer_unsubscribe(
        synchronizer, "authorengine", "project") == UMI_STATUS_OK);

    umi_desktop_context_synchronizer_destroy(synchronizer);
    umi_desktop_content_runtime_destroy(content);
    umi_desktop_runtime_destroy(desktop);
    umi_federation_router_destroy(federation);
    umi_application_context_hub_destroy(context_hub);
    return 0;
}
