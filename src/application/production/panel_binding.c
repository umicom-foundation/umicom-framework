/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/panel_binding.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/panel_binding.h"

#include <string.h>

UmiStatus umi_application_production_panel_bindings_build(
    const UmiApplicationProductionBinding *binding,
    const UmiProductSurfacePortfolio *surfaces,
    UmiApplicationProductionPanelBindings *out_bindings)
{
    size_t index;
    UmiStatus status;
    if (surfaces == NULL || out_bindings == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    if (binding->experience->panel_count >
        UMI_APPLICATION_PRODUCTION_MAX_PANELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_bindings, 0, sizeof(*out_bindings));
    for (index = 0U; index < binding->experience->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            &binding->experience->panels[index];
        const UmiProductSurfaceProjection *surface =
            umi_product_surface_portfolio_find(
                surfaces, binding->experience->application_id,
                panel->panel_id);
        UmiApplicationProductionPanelBinding *entry =
            &out_bindings->entries[out_bindings->count];
        entry->panel = panel;
        entry->required =
            (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U;
        if (surface != NULL) {
            entry->surface = *surface;
            entry->surface_available = 1;
            entry->covered =
                surface->coverage == UMI_PRODUCT_SURFACE_COVERED;
        }
        out_bindings->required_count += (size_t)entry->required;
        if (!entry->covered) out_bindings->uncovered_count += 1U;
        out_bindings->count += 1U;
    }
    return UMI_STATUS_OK;
}

const UmiApplicationProductionPanelBinding *
umi_application_production_panel_bindings_find(
    const UmiApplicationProductionPanelBindings *bindings,
    const char *panel_id)
{
    size_t index;
    if (bindings == NULL || panel_id == NULL) return NULL;
    for (index = 0U; index < bindings->count; ++index)
        if (bindings->entries[index].panel != NULL &&
            strcmp(bindings->entries[index].panel->panel_id, panel_id) == 0)
            return &bindings->entries[index];
    return NULL;
}

