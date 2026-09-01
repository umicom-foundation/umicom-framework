/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_launch_guidance.c
 *
 * PURPOSE:
 *   Verify a multi-application selection becomes a validated, non-executing
 *   launch preview with start, activation, warning and unavailable guidance.
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

#include "umicom/application/productisation/launch_guidance.h"

/* Build a small runtime declaration without copying launcher implementation. */
static UmiApplicationRuntimeRegistration test_registration(
    const char *application_id,
    const char *display_name,
    int installed)
{
    UmiApplicationRuntimeRegistration registration = {0};

    registration.structure_size = (uint32_t)sizeof(registration);
    registration.application_id = application_id;
    registration.display_name = display_name;
    registration.executable_name = "test-application";
    registration.working_directory = "";
    registration.icon_resource_id = "";
    registration.default_layout_id = "default";
    registration.taskbar_group = "applications";
    registration.family = UMI_APPLICATION_FAMILY_DEVELOPMENT;
    registration.maturity = UMI_APPLICATION_AVAILABLE;
    registration.entry_kind = UMI_APPLICATION_ENTRY_WORKBENCH;
    registration.installed = installed != 0;
    registration.compatible = true;
    registration.enabled = true;
    registration.visible_when_unavailable = true;
    return registration;
}

/* Exercise the complete preview without invoking an application launcher. */
int main(void)
{
    const UmiProductApplicationAdoption studio_adoption = {
        (uint32_t)sizeof(UmiProductApplicationAdoption), "studio",
        "org.umicom.studio", "Umicom Studio IDE", "umicom-studio-ide",
        UMI_PRODUCT_FRONTEND_FLAG_GTK4, 1, 1, 1, 1
    };
    const UmiProductApplicationAdoption trader_adoption = {
        (uint32_t)sizeof(UmiProductApplicationAdoption), "trader",
        "org.umicom.trader", "Umicom Trader", "umicom-trader",
        UMI_PRODUCT_FRONTEND_FLAG_GTK4, 1, 1, 1, 1
    };
    UmiApplicationRuntimeRegistration studio = test_registration(
        "org.umicom.studio", "Umicom Studio IDE", 1);
    UmiApplicationRuntimeRegistration trader = test_registration(
        "org.umicom.trader", "Umicom Trader", 1);
    UmiApplicationRuntimeRegistration bank = test_registration(
        "org.umicom.bank", "Umicom Bank", 1);
    UmiApplicationRuntimeRegistration unavailable = test_registration(
        "org.umicom.unavailable", "Unavailable Example", 0);
    UmiApplicationRuntimeCatalogue *catalogue = NULL;
    UmiApplicationLaunchSelection *selection = NULL;
    UmiProductAdoptionRegistry registry;
    UmiProductWorkspaceGuidePortfolio portfolio;
    UmiProductGuidedLaunchPlan plan;
    const UmiProductGuidedLaunchEntry *entry;

    umi_product_adoption_registry_init(&registry);
    assert(umi_product_adoption_registry_register(
        &registry, &studio_adoption) == UMI_STATUS_OK);
    assert(umi_product_adoption_registry_register(
        &registry, &trader_adoption) == UMI_STATUS_OK);
    assert(umi_product_workspace_guide_portfolio_build(
        &registry, &portfolio) == UMI_STATUS_OK);

    assert(umi_application_runtime_catalogue_create(&catalogue) ==
           UMI_STATUS_OK);
    assert(umi_application_runtime_catalogue_register(
        catalogue, &studio) == UMI_STATUS_OK);
    assert(umi_application_runtime_catalogue_register(
        catalogue, &trader) == UMI_STATUS_OK);
    assert(umi_application_runtime_catalogue_register(
        catalogue, &bank) == UMI_STATUS_OK);
    assert(umi_application_runtime_catalogue_register(
        catalogue, &unavailable) == UMI_STATUS_OK);

    /* A process token changes Trader from a start request to activation. */
    assert(umi_application_runtime_catalogue_set_process(
        catalogue, "org.umicom.trader", 42U) == UMI_STATUS_OK);
    assert(umi_application_launch_selection_create(
        catalogue, &selection) == UMI_STATUS_OK);
    assert(umi_application_launch_selection_set_selected(
        selection, "org.umicom.studio", true) == UMI_STATUS_OK);
    assert(umi_application_launch_selection_set_selected(
        selection, "org.umicom.trader", true) == UMI_STATUS_OK);
    /* Bank deliberately has no workspace guide, but remains launchable. */
    assert(umi_application_launch_selection_set_selected(
        selection, "org.umicom.bank", true) == UMI_STATUS_OK);

    assert(umi_product_guided_launch_plan_build(
        selection, &portfolio, &plan) == UMI_STATUS_OK);
    assert(umi_product_guided_launch_plan_validate(&plan) == UMI_STATUS_OK);
    assert(plan.entry_count == 4U);
    assert(plan.selected_count == 3U);
    assert(plan.ready_to_execute_count == 3U);
    assert(plan.start_count == 2U);
    assert(plan.activate_count == 1U);
    assert(plan.unavailable_count == 1U);
    assert(plan.missing_guidance_count == 2U);
    assert(plan.guidance_warning_count == 1U);
    assert(plan.executable);

    entry = umi_product_guided_launch_plan_find(
        &plan, "org.umicom.studio");
    assert(entry != NULL);
    assert(entry->guidance_state ==
           UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_START);
    assert(entry->resolved_action == UMI_APPLICATION_LAUNCH_START);
    assert(entry->recommended_layout_id[0] != '\0');

    entry = umi_product_guided_launch_plan_find(
        &plan, "org.umicom.trader");
    assert(entry != NULL);
    assert(entry->guidance_state ==
           UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_ACTIVATE);
    assert(entry->resolved_action == UMI_APPLICATION_LAUNCH_ACTIVATE);

    entry = umi_product_guided_launch_plan_find(
        &plan, "org.umicom.bank");
    assert(entry != NULL);
    assert(entry->guidance_state ==
           UMI_PRODUCT_LAUNCH_GUIDANCE_MISSING_GUIDE);
    assert(entry->ready_to_execute);
    assert(entry->explanation[0] != '\0');

    entry = umi_product_guided_launch_plan_find(
        &plan, "org.umicom.unavailable");
    assert(entry != NULL);
    assert(entry->guidance_state ==
           UMI_PRODUCT_LAUNCH_GUIDANCE_UNAVAILABLE);
    assert(!entry->ready_to_execute);
    assert(strcmp(umi_product_launch_guidance_state_text(
                      entry->guidance_state),
                  "unavailable") == 0);
    assert(umi_product_guided_launch_plan_at(&plan, 4U) == NULL);

    /* Aggregate tampering is rejected before a launcher renders the plan. */
    plan.selected_count += 1U;
    assert(umi_product_guided_launch_plan_validate(&plan) ==
           UMI_STATUS_INVALID_ARGUMENT);

    umi_application_launch_selection_destroy(selection);
    umi_application_runtime_catalogue_destroy(catalogue);
    return 0;
}
