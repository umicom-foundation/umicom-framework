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
#include <stdlib.h>
#include <string.h>

#include "umicom/application/productisation/launch_guidance.h"
#include "umicom/test_runtime/check.h"

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
    UmiProductWorkspaceGuidePortfolio *portfolio =
        (UmiProductWorkspaceGuidePortfolio *)calloc(1U, sizeof(*portfolio));
    UmiProductGuidedLaunchPlan *plan =
        (UmiProductGuidedLaunchPlan *)calloc(1U, sizeof(*plan));
    const UmiProductGuidedLaunchEntry *entry;

    /* These catalogue-wide values grow when applications are added, so the
     * test keeps them on the heap instead of consuming the Windows stack. */
    UMI_TEST_REQUIRE(portfolio != NULL && plan != NULL);
    umi_product_adoption_registry_init(&registry);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(
        &registry, &studio_adoption) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(
        &registry, &trader_adoption) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_portfolio_build(
        &registry, portfolio) == UMI_STATUS_OK);

    UMI_TEST_REQUIRE(umi_application_runtime_catalogue_create(&catalogue) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_runtime_catalogue_register(
        catalogue, &studio) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_runtime_catalogue_register(
        catalogue, &trader) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_runtime_catalogue_register(
        catalogue, &bank) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_runtime_catalogue_register(
        catalogue, &unavailable) == UMI_STATUS_OK);

    /* A process token changes Trader from a start request to activation. */
    UMI_TEST_REQUIRE(umi_application_runtime_catalogue_set_process(
        catalogue, "org.umicom.trader", 42U) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_launch_selection_create(
        catalogue, &selection) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_launch_selection_set_selected(
        selection, "org.umicom.studio", true) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_application_launch_selection_set_selected(
        selection, "org.umicom.trader", true) == UMI_STATUS_OK);
    /* Bank deliberately has no workspace guide, but remains launchable. */
    UMI_TEST_REQUIRE(umi_application_launch_selection_set_selected(
        selection, "org.umicom.bank", true) == UMI_STATUS_OK);

    UMI_TEST_REQUIRE(umi_product_guided_launch_plan_build(
        selection, portfolio, plan) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_guided_launch_plan_validate(plan) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(plan->entry_count == 4U);
    UMI_TEST_REQUIRE(plan->selected_count == 3U);
    UMI_TEST_REQUIRE(plan->ready_to_execute_count == 3U);
    UMI_TEST_REQUIRE(plan->start_count == 2U);
    UMI_TEST_REQUIRE(plan->activate_count == 1U);
    UMI_TEST_REQUIRE(plan->unavailable_count == 1U);
    UMI_TEST_REQUIRE(plan->missing_guidance_count == 2U);
    UMI_TEST_REQUIRE(plan->guidance_warning_count == 1U);
    UMI_TEST_REQUIRE(plan->executable);

    entry = umi_product_guided_launch_plan_find(
        plan, "org.umicom.studio");
    UMI_TEST_REQUIRE(entry != NULL);
    UMI_TEST_REQUIRE(entry->guidance_state ==
                     UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_START);
    UMI_TEST_REQUIRE(entry->resolved_action == UMI_APPLICATION_LAUNCH_START);
    UMI_TEST_REQUIRE(entry->recommended_layout_id[0] != '\0');

    entry = umi_product_guided_launch_plan_find(
        plan, "org.umicom.trader");
    UMI_TEST_REQUIRE(entry != NULL);
    UMI_TEST_REQUIRE(entry->guidance_state ==
                     UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_ACTIVATE);
    UMI_TEST_REQUIRE(entry->resolved_action == UMI_APPLICATION_LAUNCH_ACTIVATE);

    entry = umi_product_guided_launch_plan_find(
        plan, "org.umicom.bank");
    UMI_TEST_REQUIRE(entry != NULL);
    UMI_TEST_REQUIRE(entry->guidance_state ==
                     UMI_PRODUCT_LAUNCH_GUIDANCE_MISSING_GUIDE);
    UMI_TEST_REQUIRE(entry->ready_to_execute);
    UMI_TEST_REQUIRE(entry->explanation[0] != '\0');

    entry = umi_product_guided_launch_plan_find(
        plan, "org.umicom.unavailable");
    UMI_TEST_REQUIRE(entry != NULL);
    UMI_TEST_REQUIRE(entry->guidance_state ==
                     UMI_PRODUCT_LAUNCH_GUIDANCE_UNAVAILABLE);
    UMI_TEST_REQUIRE(!entry->ready_to_execute);
    UMI_TEST_REQUIRE(strcmp(umi_product_launch_guidance_state_text(
                                entry->guidance_state),
                            "unavailable") == 0);
    UMI_TEST_REQUIRE(umi_product_guided_launch_plan_at(plan, 4U) == NULL);

    /* Aggregate tampering is rejected before a launcher renders the plan. */
    plan->selected_count += 1U;
    UMI_TEST_REQUIRE(umi_product_guided_launch_plan_validate(plan) ==
                     UMI_STATUS_INVALID_ARGUMENT);

    umi_application_launch_selection_destroy(selection);
    umi_application_runtime_catalogue_destroy(catalogue);
    free(plan);
    free(portfolio);
    return 0;
}
