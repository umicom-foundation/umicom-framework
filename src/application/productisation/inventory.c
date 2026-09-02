/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/inventory.c
 *
 * PURPOSE:
 *   Build a complete immutable inventory from the existing experience
 *   catalogue instead of maintaining another product feature list.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/inventory.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/experience_status.h"

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the count feature owner operation used by this module and its client
 * applications.
 */
static void count_feature_owner(const UmiExperienceFeatureDefinition *feature,
                                UmiProductApplicationInventoryEntry *entry)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (feature->owner == UMI_EXPERIENCE_OWNER_FRAMEWORK)
        entry->framework_owned_count += 1U;
    else /* Apply this branch only when its contract condition is satisfied. */ if (feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION)
        entry->application_owned_count += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else entry->external_adapter_count += 1U;
}

/*
 * Provide the product portfolio inventory build operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_portfolio_inventory_build(
    UmiProductPortfolioInventory *out_inventory)
{
    size_t application_index;
    const size_t application_count =
        umi_application_experience_catalogue_count();

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_inventory == NULL ||
        application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_experience_catalogue_validate() != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;

    (void)memset(out_inventory, 0, sizeof(*out_inventory));
    /* Visit each bounded item once so every record receives the same rule. */
    for (application_index = 0U; application_index < application_count;
         ++application_index) {
        const UmiApplicationExperienceDefinition *definition =
            umi_application_experience_catalogue_at(application_index);
        UmiProductApplicationInventoryEntry *entry =
            &out_inventory->applications[out_inventory->application_count];
        UmiApplicationExperienceStatus status;
        size_t feature_index;
        size_t panel_index;
        UmiStatus result;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (definition == NULL) return UMI_STATUS_INVALID_STATE;
        result = umi_application_experience_status(definition, &status);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
        result = copy_text(entry->application_id,
                           sizeof(entry->application_id),
                           definition->application_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
        result = copy_text(entry->display_name, sizeof(entry->display_name),
                           definition->display_name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;
        result = copy_text(entry->default_layout_id,
                           sizeof(entry->default_layout_id),
                           definition->default_layout_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result != UMI_STATUS_OK) return result;

        entry->feature_count = status.feature_count;
        entry->panel_count = status.panel_count;
        entry->layout_count = status.layout_count;
        entry->planned_count = status.planned_count;
        entry->foundation_count = status.foundation_count;
        entry->implemented_count = status.implemented_count;
        entry->verified_count = status.verified_count;
        entry->readiness_percent = status.readiness_percent;

        /* Visit each bounded item once so every record receives the same rule. */
        for (feature_index = 0U; feature_index < definition->feature_count;
             ++feature_index)
            count_feature_owner(&definition->features[feature_index], entry);
        /* Visit each bounded item once so every record receives the same rule. */
        for (panel_index = 0U; panel_index < definition->panel_count;
             ++panel_index) {
            /* Apply this branch only when its contract condition is satisfied. */
            if ((definition->panels[panel_index].flags &
                 UMI_EXPERIENCE_PANEL_CRITICAL) != 0U)
                entry->critical_panel_count += 1U;
        }

        out_inventory->application_count += 1U;
        out_inventory->feature_count += entry->feature_count;
        out_inventory->panel_count += entry->panel_count;
        out_inventory->layout_count += entry->layout_count;
        out_inventory->planned_count += entry->planned_count;
        out_inventory->foundation_count += entry->foundation_count;
        out_inventory->implemented_count += entry->implemented_count;
        out_inventory->verified_count += entry->verified_count;
        out_inventory->framework_owned_count += entry->framework_owned_count;
        out_inventory->application_owned_count += entry->application_owned_count;
        out_inventory->external_adapter_count += entry->external_adapter_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Find product portfolio inventory while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiProductApplicationInventoryEntry *
umi_product_portfolio_inventory_find(
    const UmiProductPortfolioInventory *inventory,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->application_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(inventory->applications[index].application_id,
                   application_id) == 0)
            return &inventory->applications[index];
    }
    return NULL;
}
