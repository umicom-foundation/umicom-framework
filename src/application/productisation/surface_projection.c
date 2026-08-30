/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/surface_projection.c
 *
 * PURPOSE:
 *   Resolve canonical application panels to Framework component definitions
 *   and publish deterministic portfolio-wide surface coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/surface_projection.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    source = source != NULL ? source : "";
    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus initialise_projection(
    UmiProductSurfaceProjection *projection,
    const UmiApplicationExperienceDefinition *application,
    const UmiExperiencePanelDefinition *panel)
{
    const UmiApplicationComponentDefinition *component =
        umi_application_component_capability_at(
            panel->required_capability, 0U);
    UmiStatus status;

    (void)memset(projection, 0, sizeof(*projection));
    status = copy_text(projection->application_id,
                       sizeof(projection->application_id),
                       application->application_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->panel_id, sizeof(projection->panel_id),
                       panel->panel_id);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->panel_title,
                       sizeof(projection->panel_title), panel->title);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->capability_id,
                       sizeof(projection->capability_id),
                       panel->required_capability);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->default_region,
                       sizeof(projection->default_region),
                       panel->default_region);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->context_group_id,
                       sizeof(projection->context_group_id),
                       panel->context_group_id);
    if (status != UMI_STATUS_OK) return status;

    projection->panel_flags = panel->flags;
    if (component == NULL) {
        projection->coverage = UMI_PRODUCT_SURFACE_MISSING_COMPONENT;
        return UMI_STATUS_OK;
    }
    status = copy_text(projection->component_id,
                       sizeof(projection->component_id),
                       component->component_id);
    if (status != UMI_STATUS_OK) return status;
    projection->component_role = component->role;
    projection->component_maturity = component->maturity;
    projection->coverage = UMI_PRODUCT_SURFACE_COVERED;
    return UMI_STATUS_OK;
}

UmiStatus umi_product_surface_portfolio_build(
    UmiProductSurfacePortfolio *out_portfolio)
{
    size_t application_index;
    const size_t application_count =
        umi_application_experience_catalogue_count();

    if (out_portfolio == NULL ||
        application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_portfolio, 0, sizeof(*out_portfolio));

    for (application_index = 0U; application_index < application_count;
         ++application_index) {
        const UmiApplicationExperienceDefinition *application =
            umi_application_experience_catalogue_at(application_index);
        UmiProductApplicationSurfaceSummary *summary =
            &out_portfolio->applications[out_portfolio->application_count];
        size_t panel_index;
        UmiStatus status;

        if (application == NULL) return UMI_STATUS_INVALID_STATE;
        status = copy_text(summary->application_id,
                           sizeof(summary->application_id),
                           application->application_id);
        if (status != UMI_STATUS_OK) return status;
        out_portfolio->application_count += 1U;

        for (panel_index = 0U; panel_index < application->panel_count;
             ++panel_index) {
            UmiProductSurfaceProjection *projection;
            const UmiExperiencePanelDefinition *panel =
                &application->panels[panel_index];
            if (out_portfolio->surface_count >=
                UMI_PRODUCTISATION_MAX_SURFACES)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            projection = &out_portfolio->surfaces[
                out_portfolio->surface_count++];
            status = initialise_projection(projection, application, panel);
            if (status != UMI_STATUS_OK) return status;
            summary->surface_count += 1U;
            if (projection->coverage == UMI_PRODUCT_SURFACE_COVERED) {
                summary->covered_count += 1U;
                out_portfolio->covered_count += 1U;
            } else {
                summary->missing_count += 1U;
                out_portfolio->missing_count += 1U;
            }
            if ((panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U) {
                summary->critical_count += 1U;
                out_portfolio->critical_count += 1U;
            }
            if ((panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) != 0U) {
                summary->context_linked_count += 1U;
                out_portfolio->context_linked_count += 1U;
            }
            if ((panel->flags & UMI_EXPERIENCE_PANEL_MULTI_MONITOR) != 0U) {
                summary->multi_monitor_count += 1U;
                out_portfolio->multi_monitor_count += 1U;
            }
        }
    }
    return UMI_STATUS_OK;
}

const UmiProductSurfaceProjection *umi_product_surface_portfolio_at(
    const UmiProductSurfacePortfolio *portfolio,
    size_t index)
{
    return portfolio != NULL && index < portfolio->surface_count
        ? &portfolio->surfaces[index] : NULL;
}

const UmiProductSurfaceProjection *umi_product_surface_portfolio_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id,
    const char *panel_id)
{
    size_t index;
    if (portfolio == NULL || application_id == NULL || panel_id == NULL)
        return NULL;
    for (index = 0U; index < portfolio->surface_count; ++index) {
        const UmiProductSurfaceProjection *projection =
            &portfolio->surfaces[index];
        if (strcmp(projection->application_id, application_id) == 0 &&
            strcmp(projection->panel_id, panel_id) == 0)
            return projection;
    }
    return NULL;
}

const UmiProductApplicationSurfaceSummary *
umi_product_surface_portfolio_application_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id)
{
    size_t index;
    if (portfolio == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < portfolio->application_count; ++index) {
        if (strcmp(portfolio->applications[index].application_id,
                   application_id) == 0)
            return &portfolio->applications[index];
    }
    return NULL;
}

const char *umi_product_surface_coverage_text(
    UmiProductSurfaceCoverage coverage)
{
    switch (coverage) {
        case UMI_PRODUCT_SURFACE_COVERED: return "covered";
        case UMI_PRODUCT_SURFACE_MISSING_COMPONENT: return "missing-component";
        default: return "unknown";
    }
}
