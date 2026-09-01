/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_adoption_registry.c
 *
 * PURPOSE:
 *   Verify bounded product contribution registration, duplicate rejection and
 *   portfolio adoption reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_runtime/check.h"

#include "umicom/application/productisation/adoption_registry.h"

int main(void)
{
    const UmiProductApplicationAdoption studio = {
        sizeof(UmiProductApplicationAdoption), "studio",
        "org.umicom.studio", "Umicom Studio IDE", "umicom-studio-ide",
        UMI_PRODUCT_FRONTEND_FLAG_GTK4, 1, 1, 1, 1
    };
    const UmiProductApplicationAdoption trader = {
        sizeof(UmiProductApplicationAdoption), "trader",
        "org.umicom.trader", "Umicom Trader", "umicom-trader-console",
        UMI_PRODUCT_FRONTEND_FLAG_CONSOLE, 1, 1, 1, 1
    };
    UmiProductAdoptionRegistry registry;
    UmiProductAdoptionRegistryReport report;
    umi_product_adoption_registry_init(&registry);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(&registry, &studio) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(&registry, &trader) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(&registry, &studio) ==
           UMI_STATUS_ALREADY_EXISTS);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_find(
        &registry, "org.umicom.trader") == &trader);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_report(
        &registry, &report) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(report.contribution_count == 2U);
    UMI_TEST_REQUIRE(report.canonical_count == 2U);
    UMI_TEST_REQUIRE(report.runnable_count == 2U);
    UMI_TEST_REQUIRE(report.tested_count == 2U);
    UMI_TEST_REQUIRE(report.layout_ready_count == 2U);
    UMI_TEST_REQUIRE(report.surface_complete_count == 2U);
    UMI_TEST_REQUIRE(report.accepted_count == 2U);
    UMI_TEST_REQUIRE(report.invalid_count == 0U);
    return 0;
}
