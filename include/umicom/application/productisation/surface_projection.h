/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/surface_projection.h
 *
 * PURPOSE:
 *   Project every canonical application panel onto a reusable Framework
 *   component without introducing product-owned UI implementations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_SURFACE_PROJECTION_H
#define UMICOM_APPLICATION_PRODUCTISATION_SURFACE_PROJECTION_H

#include "umicom/application/component_catalogue.h"
#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PRODUCTISATION_MAX_SURFACES 512U

typedef enum UmiProductSurfaceCoverage {
    UMI_PRODUCT_SURFACE_COVERED = 1,
    UMI_PRODUCT_SURFACE_MISSING_COMPONENT = 2
} UmiProductSurfaceCoverage;

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

typedef struct UmiProductApplicationSurfaceSummary {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    size_t surface_count;
    size_t covered_count;
    size_t missing_count;
    size_t critical_count;
    size_t context_linked_count;
    size_t multi_monitor_count;
} UmiProductApplicationSurfaceSummary;

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

UmiStatus umi_product_surface_portfolio_build(
    UmiProductSurfacePortfolio *out_portfolio);
const UmiProductSurfaceProjection *umi_product_surface_portfolio_at(
    const UmiProductSurfacePortfolio *portfolio,
    size_t index);
const UmiProductSurfaceProjection *umi_product_surface_portfolio_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id,
    const char *panel_id);
const UmiProductApplicationSurfaceSummary *
umi_product_surface_portfolio_application_find(
    const UmiProductSurfacePortfolio *portfolio,
    const char *application_id);
const char *umi_product_surface_coverage_text(
    UmiProductSurfaceCoverage coverage);

#ifdef __cplusplus
}
#endif

#endif
