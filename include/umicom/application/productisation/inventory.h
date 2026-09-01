/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/inventory.h
 *
 * PURPOSE:
 *   Project the canonical application experience catalogue into one bounded
 *   inventory of features, layouts, panels and ownership responsibilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_INVENTORY_H
#define UMICOM_APPLICATION_PRODUCTISATION_INVENTORY_H

#include "umicom/application/experience.h"
#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Count features, panels, layouts, maturity and ownership for one application. */
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

/** Aggregate the canonical application catalogue without copying its definitions. */
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

/** Build a bounded inventory directly from the canonical experience catalogue. */
UmiStatus umi_product_portfolio_inventory_build(
    UmiProductPortfolioInventory *out_inventory);
/** Borrow one inventory row by exact canonical application identifier. */
const UmiProductApplicationInventoryEntry *
umi_product_portfolio_inventory_find(
    const UmiProductPortfolioInventory *inventory,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
