/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/workspace_guide_portfolio.h
 *
 * PURPOSE:
 *   Summarise registered application workspace guides for suite launchers,
 *   portfolio dashboards and cross-application readiness views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_WORKSPACE_GUIDE_PORTFOLIO_H
#define UMICOM_APPLICATION_PRODUCTISATION_WORKSPACE_GUIDE_PORTFOLIO_H

#include "umicom/application/productisation/adoption_registry.h"
#include "umicom/application/productisation/workspace_guide.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A summary keeps launcher memory small while retaining useful guidance. */
typedef struct UmiProductWorkspaceGuideSummary {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char display_name[UMI_PRODUCTISATION_ID_CAPACITY];
    char executable_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char recommended_layout_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char next_feature_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char next_feature_title[UMI_PRODUCTISATION_ID_CAPACITY];
    size_t layout_choice_count;
    size_t total_panel_placements;
    unsigned readiness_percent;
    int has_next_feature;
    int acceptance_ready;
} UmiProductWorkspaceGuideSummary;

/* The portfolio is bounded by the existing application contribution limit. */
typedef struct UmiProductWorkspaceGuidePortfolio {
    uint32_t structure_size;
    UmiProductWorkspaceGuideSummary
        applications[UMI_PRODUCTISATION_MAX_APPLICATIONS];
    size_t application_count;
    size_t layout_choice_count;
    size_t panel_placement_count;
    size_t applications_with_next_feature;
    size_t acceptance_ready_count;
    unsigned average_readiness_percent;
} UmiProductWorkspaceGuidePortfolio;

/** Build summaries for every application registered by a suite launcher. */
UmiStatus umi_product_workspace_guide_portfolio_build(
    const UmiProductAdoptionRegistry *registry,
    UmiProductWorkspaceGuidePortfolio *out_portfolio);

/** Validate totals, identities and readiness before presenting the portfolio. */
UmiStatus umi_product_workspace_guide_portfolio_validate(
    const UmiProductWorkspaceGuidePortfolio *portfolio);

/** Return one borrowed application summary when the index is in range. */
const UmiProductWorkspaceGuideSummary *
umi_product_workspace_guide_portfolio_at(
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    size_t index);

/** Find a borrowed summary by its canonical application identifier. */
const UmiProductWorkspaceGuideSummary *
umi_product_workspace_guide_portfolio_find(
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    const char *application_id);

#ifdef __cplusplus
}
#endif

#endif
