/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/inventory.c
 *
 * PURPOSE:
 *   Build a complete immutable inventory from the existing experience
 *   catalogue instead of maintaining another product feature list.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/inventory.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/experience_status.h"

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static void count_feature_owner(const UmiExperienceFeatureDefinition *feature,
                                UmiProductApplicationInventoryEntry *entry)
{
    if (feature->owner == UMI_EXPERIENCE_OWNER_FRAMEWORK)
        entry->framework_owned_count += 1U;
    else if (feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION)
        entry->application_owned_count += 1U;
    else entry->external_adapter_count += 1U;
}

UmiStatus umi_product_portfolio_inventory_build(
    UmiProductPortfolioInventory *out_inventory)
{
    size_t application_index;
    const size_t application_count =
        umi_application_experience_catalogue_count();

    if (out_inventory == NULL ||
        application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_catalogue_validate() != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_STATE;

    (void)memset(out_inventory, 0, sizeof(*out_inventory));
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

        if (definition == NULL) return UMI_STATUS_INVALID_STATE;
        result = umi_application_experience_status(definition, &status);
        if (result != UMI_STATUS_OK) return result;
        result = copy_text(entry->application_id,
                           sizeof(entry->application_id),
                           definition->application_id);
        if (result != UMI_STATUS_OK) return result;
        result = copy_text(entry->display_name, sizeof(entry->display_name),
                           definition->display_name);
        if (result != UMI_STATUS_OK) return result;
        result = copy_text(entry->default_layout_id,
                           sizeof(entry->default_layout_id),
                           definition->default_layout_id);
        if (result != UMI_STATUS_OK) return result;

        entry->feature_count = status.feature_count;
        entry->panel_count = status.panel_count;
        entry->layout_count = status.layout_count;
        entry->planned_count = status.planned_count;
        entry->foundation_count = status.foundation_count;
        entry->implemented_count = status.implemented_count;
        entry->verified_count = status.verified_count;
        entry->readiness_percent = status.readiness_percent;

        for (feature_index = 0U; feature_index < definition->feature_count;
             ++feature_index)
            count_feature_owner(&definition->features[feature_index], entry);
        for (panel_index = 0U; panel_index < definition->panel_count;
             ++panel_index) {
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

const UmiProductApplicationInventoryEntry *
umi_product_portfolio_inventory_find(
    const UmiProductPortfolioInventory *inventory,
    const char *application_id)
{
    size_t index;
    if (inventory == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < inventory->application_count; ++index) {
        if (strcmp(inventory->applications[index].application_id,
                   application_id) == 0)
            return &inventory->applications[index];
    }
    return NULL;
}
