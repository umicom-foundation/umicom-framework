/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/surface_projection.h
 *
 * PURPOSE:
 *   Project every canonical application panel onto a reusable Framework
 *   component without introducing product-owned UI implementations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_SURFACE_PROJECTION_H
#define UMICOM_APPLICATION_PRODUCTISATION_SURFACE_PROJECTION_H

#include "umicom/application/component_catalogue.h"
#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCTISATION_MAX_SURFACES 512U

/** Explain whether a panel resolved to a reusable component. */
typedef enum UmiProductSurfaceCoverage {
    UMI_PRODUCT_SURFACE_COVERED = 1,
    UMI_PRODUCT_SURFACE_MISSING_COMPONENT = 2
} UmiProductSurfaceCoverage;

/** Record the component, placement and context selected for one panel. */
typedef struct UmiProductSurfaceProjection {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char panel_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char panel_title[UMI_PRODUCTISATION_ID_CAPACITY];
    char capability_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char component_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char default_region[UMI_PRODUCTISATION_ID_CAPACITY];
    char context_group_id[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiApplicationComponentRole component_role;
    UmiCapabilityMaturity component_maturity;
    uint32_t panel_flags;
    UmiProductSurfaceCoverage coverage;
} UmiProductSurfaceProjection;

/** Summarise surface coverage and layout abilities for one application. */
typedef struct UmiProductApplicationSurfaceSummary {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    size_t surface_count;
    size_t covered_count;
    size_t missing_count;
    size_t critical_count;
    size_t context_linked_count;
    size_t multi_monitor_count;
} UmiProductApplicationSurfaceSummary;

/** Hold all bounded panel projections and per-application summaries. */
typedef struct UmiProductSurfacePortfolio {
    UmiProductSurfaceProjection surfaces[UMI_PRODUCTISATION_MAX_SURFACES];
    UmiProductApplicationSurfaceSummary
        applications[UMI_PRODUCTISATION_MAX_APPLICATIONS];
    size_t surface_count;
    size_t application_count;
    size_t covered_count;
    size_t missing_count;
    size_t critical_count;
    size_t context_linked_count;
    size_t multi_monitor_count;
} UmiProductSurfacePortfolio;

/** Project every canonical panel through the reusable component catalogue. */
UmiStatus umi_product_surface_portfolio_build(
    UmiProductSurfacePortfolio *out_portfolio);
/** Borrow the surface at index, or return NULL for an invalid index. */
const UmiProductSurfaceProjection *umi_product_surface_portfolio_at(
    const UmiProductSurfacePortfolio *portfolio,
    size_t index);
/** Borrow a surface using its application and panel identifiers. */
const UmiProductSurfaceProjection *umi_product_surface_portfolio_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id,
    const char *panel_id);
/** Borrow one application's aggregate surface summary. */
const UmiProductApplicationSurfaceSummary *
umi_product_surface_portfolio_application_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id);
/** Return stable diagnostic text for a surface coverage state. */
const char *umi_product_surface_coverage_text(
    UmiProductSurfaceCoverage coverage);

#ifdef __cplusplus
}
#endif

#endif
