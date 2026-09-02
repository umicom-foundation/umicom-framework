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

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    source = source != NULL ? source : "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the initialise projection operation used by this module and its client
 * applications.
 */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->panel_id, sizeof(projection->panel_id),
                       panel->panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->panel_title,
                       sizeof(projection->panel_title), panel->title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->capability_id,
                       sizeof(projection->capability_id),
                       panel->required_capability);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->default_region,
                       sizeof(projection->default_region),
                       panel->default_region);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(projection->context_group_id,
                       sizeof(projection->context_group_id),
                       panel->context_group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    projection->panel_flags = panel->flags;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (component == NULL) {
        projection->coverage = UMI_PRODUCT_SURFACE_MISSING_COMPONENT;
        return UMI_STATUS_OK;
    }
    status = copy_text(projection->component_id,
                       sizeof(projection->component_id),
                       component->component_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    projection->component_role = component->role;
    projection->component_maturity = component->maturity;
    projection->coverage = UMI_PRODUCT_SURFACE_COVERED;
    return UMI_STATUS_OK;
}

/*
 * Provide the product surface portfolio build operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_surface_portfolio_build(
    UmiProductSurfacePortfolio *out_portfolio)
{
    size_t application_index;
    const size_t application_count =
        umi_application_experience_catalogue_count();

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_portfolio == NULL ||
        application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_portfolio, 0, sizeof(*out_portfolio));

    /* Visit each bounded item once so every record receives the same rule. */
    for (application_index = 0U; application_index < application_count;
         ++application_index) {
        const UmiApplicationExperienceDefinition *application =
            umi_application_experience_catalogue_at(application_index);
        UmiProductApplicationSurfaceSummary *summary =
            &out_portfolio->applications[out_portfolio->application_count];
        size_t panel_index;
        UmiStatus status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (application == NULL) return UMI_STATUS_INVALID_STATE;
        status = copy_text(summary->application_id,
                           sizeof(summary->application_id),
                           application->application_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_portfolio->application_count += 1U;

        /* Visit each bounded item once so every record receives the same rule. */
        for (panel_index = 0U; panel_index < application->panel_count;
             ++panel_index) {
            UmiProductSurfaceProjection *projection;
            const UmiExperiencePanelDefinition *panel =
                &application->panels[panel_index];
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (out_portfolio->surface_count >=
                UMI_PRODUCTISATION_MAX_SURFACES)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            projection = &out_portfolio->surfaces[
                out_portfolio->surface_count++];
            status = initialise_projection(projection, application, panel);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            summary->surface_count += 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (projection->coverage == UMI_PRODUCT_SURFACE_COVERED) {
                summary->covered_count += 1U;
                out_portfolio->covered_count += 1U;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                summary->missing_count += 1U;
                out_portfolio->missing_count += 1U;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if ((panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U) {
                summary->critical_count += 1U;
                out_portfolio->critical_count += 1U;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if ((panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) != 0U) {
                summary->context_linked_count += 1U;
                out_portfolio->context_linked_count += 1U;
            }
            /* Apply this branch only when its contract condition is satisfied. */
            if ((panel->flags & UMI_EXPERIENCE_PANEL_MULTI_MONITOR) != 0U) {
                summary->multi_monitor_count += 1U;
                out_portfolio->multi_monitor_count += 1U;
            }
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Find product surface portfolio while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiProductSurfaceProjection *umi_product_surface_portfolio_at(
    const UmiProductSurfacePortfolio *portfolio,
    size_t index)
{
    return portfolio != NULL && index < portfolio->surface_count
        ? &portfolio->surfaces[index] : NULL;
}

/*
 * Find product surface portfolio while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiProductSurfaceProjection *umi_product_surface_portfolio_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (portfolio == NULL || application_id == NULL || panel_id == NULL)
        return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < portfolio->surface_count; ++index) {
        const UmiProductSurfaceProjection *projection =
            &portfolio->surfaces[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(projection->application_id, application_id) == 0 &&
            strcmp(projection->panel_id, panel_id) == 0)
            return projection;
    }
    return NULL;
}

/*
 * Find product surface portfolio application while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiProductApplicationSurfaceSummary *
umi_product_surface_portfolio_application_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (portfolio == NULL || application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < portfolio->application_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(portfolio->applications[index].application_id,
                   application_id) == 0)
            return &portfolio->applications[index];
    }
    return NULL;
}

/*
 * Provide the product surface coverage text operation used by this module and its client
 * applications.
 */
const char *umi_product_surface_coverage_text(
    UmiProductSurfaceCoverage coverage)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (coverage) {
        case UMI_PRODUCT_SURFACE_COVERED: return "covered";
        case UMI_PRODUCT_SURFACE_MISSING_COMPONENT: return "missing-component";
        default: return "unknown";
    }
}
