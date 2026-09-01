/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application/test_application_resource_broker.c
 *
 * PURPOSE:
 *   Verify capability-checked runtime leases, the immutable shared-resource
 *   catalogue and application presentation metadata used by Umicom Desk.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/application.h"

int main(void)
{
    UmiApplicationResourceBroker *broker = NULL;
    UmiResourceLeaseRequest request = {0};
    UmiResourceLease first;
    UmiResourceLease second;
    UmiApplicationResourceLocation location;
    const UmiApplicationResourceDescriptor *resource;
    const UmiApplicationPresentation *presentation;
    const UmiApplicationDefinition *studio =
        umi_application_portfolio_find("org.umicom.studio");
    const UmiApplicationDefinition *llm =
        umi_application_portfolio_find("org.umicom.llm");

    assert(umi_application_resource_catalogue_validate() == UMI_STATUS_OK);
    assert(umi_application_resource_catalogue_count() >= 25U);
    resource = umi_application_resource_catalogue_find(
        "umicom.brand.logo.primary");
    assert(resource != NULL);
    assert(resource->kind == UMI_APPLICATION_RESOURCE_FILE);
    assert((resource->flags & UMI_APPLICATION_RESOURCE_TRADEMARK) != 0U);
    assert((resource->flags & UMI_APPLICATION_RESOURCE_REQUIRED) != 0U);
    assert((resource->flags & UMI_APPLICATION_RESOURCE_DERIVED) == 0U);
    assert(strcmp(resource->locator, "brand/umicom-logo.svg") == 0);
    resource = umi_application_resource_catalogue_find(
        "umicom.brand.logo.on-dark");
    assert(resource != NULL);
    assert(strcmp(resource->locator, "brand/umicom-logo-on-dark.svg") == 0);
    resource = umi_application_resource_catalogue_find(
        "umicom.brand.icon.on-dark");
    assert(resource != NULL);
    assert(strcmp(resource->locator, "brand/umicom-icon-on-dark.svg") == 0);
    resource = umi_application_resource_catalogue_find(
        "umicom.brand.icon.raster");
    assert(resource != NULL);
    assert(strcmp(resource->locator, "brand/umicom-icon.png") == 0);
    assert((resource->flags & UMI_APPLICATION_RESOURCE_DERIVED) != 0U);
    assert((resource->flags & UMI_APPLICATION_RESOURCE_REQUIRED) == 0U);
    resource = umi_application_resource_catalogue_find(
        "umicom.brand.icon.windows");
    assert(resource != NULL);
    assert((resource->flags & UMI_APPLICATION_RESOURCE_DERIVED) != 0U);
    resource = umi_application_resource_catalogue_find(
        "umicom.linux.application-desktop-template");
    assert(resource != NULL);
    assert(strcmp(resource->locator,
                  "linux/umicom-application.desktop.in") == 0);
    resource = umi_application_resource_catalogue_find("umicom.layout.mosaic");
    assert(resource != NULL);
    assert(resource->kind == UMI_APPLICATION_RESOURCE_LAYOUT_TEMPLATE);
    assert(strcmp(resource->locator,
                  "layouts/templates/mosaic.umilayout") == 0);

    assert(umi_application_resource_resolve(
        NULL, "umicom.icon.application.studio", &location) == UMI_STATUS_OK);
    assert(location.available);
    assert(location.kind == UMI_APPLICATION_RESOURCE_THEME_ICON);
    assert(strcmp(location.locator, "applications-development-symbolic") == 0);

    assert(umi_application_presentation_catalogue_validate() == UMI_STATUS_OK);
    presentation = umi_application_presentation_find("org.umicom.studio");
    assert(presentation != NULL);
    assert(presentation->pinned_by_default);
    assert(presentation->entry_kind == UMI_APPLICATION_ENTRY_WORKBENCH);
    assert(strcmp(presentation->default_layout_id, "develop") == 0);
    assert(strcmp(umi_application_entry_kind_text(presentation->entry_kind),
                  "workbench") == 0);

    assert(umi_application_resource_broker_create(&broker) == UMI_STATUS_OK);
    request.application = studio;
    request.resource_id = "accelerator.gpu-0";
    request.capability_id = "umicom.ai";
    request.mode = UMI_RESOURCE_LEASE_SHARED_READ;
    request.quota_units = 4U;
    request.expires_at = 100U;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &first) == UMI_STATUS_OK);
    request.application = llm;
    request.quota_units = 8U;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &second) == UMI_STATUS_OK);
    assert(umi_application_resource_broker_active_count(broker) == 2U);

    request.mode = UMI_RESOURCE_LEASE_EXCLUSIVE;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &second) == UMI_STATUS_BUSY);
    assert(umi_application_resource_broker_release(
        broker, studio->application_id, first.lease_id) == UMI_STATUS_OK);
    assert(umi_application_resource_broker_release(
        broker, llm->application_id, second.lease_id) == UMI_STATUS_OK);
    assert(umi_application_resource_broker_active_count(broker) == 0U);

    request.application = studio;
    request.resource_id = "broker.live";
    request.capability_id = "umicom.trading";
    request.mode = UMI_RESOURCE_LEASE_EXCLUSIVE;
    assert(umi_application_resource_broker_acquire(
        broker, &request, &first) == UMI_STATUS_PERMISSION_DENIED);
    umi_application_resource_broker_destroy(broker);
    return 0;
}
