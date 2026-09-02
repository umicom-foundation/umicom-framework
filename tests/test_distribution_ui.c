/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_ui.c
 *
 * PURPOSE:
 *   Verify toolkit-neutral Marketplace and Update Centre projections expose
 *   service data and executable action metadata.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>

#include "distribution_fixture.h"
#include "umicom/distribution_ui/distribution_ui.h"
#include "umicom/ui/command_view.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDistributionService *service = NULL;
    UmiDistributionPackage package = distribution_test_package(
        "studio-0.17.0", "org.umicom.studio", (UmiVersion){0U, 17U, 0U},
        UMI_RELEASE_STABLE);
    UmiUiViewModel *view = NULL;
    UmiUiValue value;
    UmiUiCommandViewAction action;
    assert(umi_distribution_service_create(NULL, &service) == UMI_STATUS_OK);
    assert(umi_distribution_service_publish(service, &package) == UMI_STATUS_OK);
    assert(umi_distribution_ui_marketplace_view_create(
        "studio.product-marketplace", service, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
        view, "marketplace.row-count", &value) == UMI_STATUS_OK);
    assert(value.integer_value == 1);
    assert(umi_ui_command_view_action_at(view, 0U, &action) == UMI_STATUS_OK);
    assert(action.enabled);
    assert(umi_ui_command_view_action_at(view, 4U, &action) == UMI_STATUS_OK);
    assert(action.enabled);
    umi_ui_view_model_destroy(view);
    assert(umi_distribution_ui_installed_view_create(
        "studio.products-installed", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_distribution_ui_updates_view_create(
        "studio.product-updates", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_distribution_ui_transactions_view_create(
        "studio.product-transactions", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    assert(umi_distribution_ui_evidence_view_create(
        "studio.product-evidence", service, &view) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    umi_distribution_service_destroy(service);
    return 0;
}
