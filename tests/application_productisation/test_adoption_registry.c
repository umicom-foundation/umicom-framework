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
#include <assert.h>

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
    assert(umi_product_adoption_registry_register(&registry, &studio) ==
           UMI_STATUS_OK);
    assert(umi_product_adoption_registry_register(&registry, &trader) ==
           UMI_STATUS_OK);
    assert(umi_product_adoption_registry_register(&registry, &studio) ==
           UMI_STATUS_ALREADY_EXISTS);
    assert(umi_product_adoption_registry_find(
        &registry, "org.umicom.trader") == &trader);
    assert(umi_product_adoption_registry_report(
        &registry, &report) == UMI_STATUS_OK);
    assert(report.contribution_count == 2U);
    assert(report.canonical_count == 2U);
    assert(report.runnable_count == 2U);
    assert(report.tested_count == 2U);
    assert(report.layout_ready_count == 2U);
    assert(report.surface_complete_count == 2U);
    assert(report.accepted_count == 2U);
    assert(report.invalid_count == 0U);
    return 0;
}
