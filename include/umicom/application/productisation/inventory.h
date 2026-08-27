/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/inventory.h
 *
 * PURPOSE:
 *   Project the canonical application experience catalogue into one bounded
 *   inventory of features, layouts, panels and ownership responsibilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_INVENTORY_H
#define UMICOM_APPLICATION_PRODUCTISATION_INVENTORY_H

#include "umicom/application/experience.h"
#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductApplicationInventoryEntry {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char display_name[UMI_PRODUCTISATION_ID_CAPACITY];
    char default_layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    size_t feature_count;
    size_t panel_count;
    size_t layout_count;
    size_t planned_count;
    size_t foundation_count;
    size_t implemented_count;
    size_t verified_count;
    size_t framework_owned_count;
    size_t application_owned_count;
    size_t external_adapter_count;
    size_t critical_panel_count;
    unsigned readiness_percent;
} UmiProductApplicationInventoryEntry;

typedef struct UmiProductPortfolioInventory {
    UmiProductApplicationInventoryEntry
        applications[UMI_PRODUCTISATION_MAX_APPLICATIONS];
    size_t application_count;
    size_t feature_count;
    size_t panel_count;
    size_t layout_count;
    size_t framework_owned_count;
    size_t application_owned_count;
    size_t external_adapter_count;
    size_t planned_count;
    size_t foundation_count;
    size_t implemented_count;
    size_t verified_count;
} UmiProductPortfolioInventory;

UmiStatus umi_product_portfolio_inventory_build(
    UmiProductPortfolioInventory *out_inventory);
const UmiProductApplicationInventoryEntry *
umi_product_portfolio_inventory_find(
    const UmiProductPortfolioInventory *inventory,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
