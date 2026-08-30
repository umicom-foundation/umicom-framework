/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_components/test_component_host.c
 *
 * PURPOSE:
 *   Implement the test component host behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Component host service | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/ui/components/host.h"

static UmiStatus create_test_view(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view)
{
    UmiUiValue value;
    UmiStatus status;
    (void)user_data;
    status = umi_ui_view_model_create(
        view_id, "test.component-view", UMI_UI_ROLE_PANE, out_view);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_value_set_string(&value, "Framework hosted content");
    if (status == UMI_STATUS_OK)
        status = umi_ui_view_model_set_property(*out_view, "summary", &value);
    return status;
}

int main(void)
{
    UmiUiComponentHostService *service = NULL;
    UmiUiComponentHostDescriptor descriptor = {0};
    UmiUiComponentHostSnapshot snapshot;
    UmiUiViewFactoryDescriptor factory = {0};
    UmiUiViewPresentation *presentation =
        (UmiUiViewPresentation *)calloc(1U, sizeof(*presentation));
    assert(presentation != NULL);
    assert(umi_ui_component_host_service_create(&service) == UMI_STATUS_OK);
    (void)snprintf(factory.view_type, sizeof(factory.view_type), "%s",
                   "test.component-view");
    (void)snprintf(factory.provider_id, sizeof(factory.provider_id), "%s",
                   "org.umicom.test");
    factory.create = create_test_view;
    assert(umi_ui_view_factory_register(
        umi_ui_component_host_view_factories(service), &factory) ==
        UMI_STATUS_OK);
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    (void)snprintf(descriptor.host_id, sizeof(descriptor.host_id), "%s",
                   "host-one");
    (void)snprintf(descriptor.component_id,
                   sizeof(descriptor.component_id), "%s",
                   "umicom.development.explorer");
    (void)snprintf(descriptor.owner_application_id,
                   sizeof(descriptor.owner_application_id), "%s",
                   "org.umicom.studio");
    (void)snprintf(descriptor.view_type, sizeof(descriptor.view_type), "%s",
                   "test.component-view");
    (void)snprintf(descriptor.view_id, sizeof(descriptor.view_id), "%s",
                   "test-view-one");
    (void)snprintf(descriptor.title, sizeof(descriptor.title), "%s",
                   "Hosted Test");
    descriptor.visible = 1;
    assert(umi_ui_component_host_mount(service, &descriptor) == UMI_STATUS_OK);
    assert(umi_ui_component_host_count(service) == 1U);
    assert(umi_ui_component_host_activate(service, "host-one") ==
           UMI_STATUS_OK);
    assert(umi_ui_component_host_snapshot(service, "host-one", &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_UI_COMPONENT_HOST_ACTIVE);
    assert(snapshot.presentation_property_count == 1U);
    assert(umi_ui_component_host_presentation(
        service, "host-one", presentation) == UMI_STATUS_OK);
    assert(strcmp(presentation->view.view_id, "test-view-one") == 0);
    assert(umi_ui_component_host_suspend(service, "host-one") ==
           UMI_STATUS_OK);
    assert(umi_ui_component_host_refresh(service, "host-one") ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_ui_component_host_resume(service, "host-one") ==
           UMI_STATUS_OK);
    assert(umi_ui_component_host_show(service, "host-one", 0) ==
           UMI_STATUS_OK);
    assert(umi_ui_component_host_unmount(service, "host-one") ==
           UMI_STATUS_OK);
    assert(umi_ui_component_host_count(service) == 0U);
    umi_ui_component_host_service_destroy(service);
    free(presentation);
    return 0;
}
