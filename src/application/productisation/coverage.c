/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/coverage.c
 *
 * PURPOSE:
 *   Measure product implementation and visible acceptance using canonical
 *   experience assets plus recorded evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/coverage.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"

static UmiProductisationEvidenceState feature_fallback(
    UmiExperienceFeatureState state)
{
    switch (state) {
    case UMI_EXPERIENCE_FEATURE_FOUNDATION:
        return UMI_PRODUCTISATION_EVIDENCE_FOUNDATION;
    case UMI_EXPERIENCE_FEATURE_IMPLEMENTED:
        return UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED;
    case UMI_EXPERIENCE_FEATURE_VERIFIED:
        return UMI_PRODUCTISATION_EVIDENCE_ACCEPTED;
    case UMI_EXPERIENCE_FEATURE_PLANNED:
    default:
        return UMI_PRODUCTISATION_EVIDENCE_DECLARED;
    }
}

static unsigned evidence_weight(UmiProductisationEvidenceState state)
{
    switch (state) {
    case UMI_PRODUCTISATION_EVIDENCE_FOUNDATION: return 30U;
    case UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED: return 55U;
    case UMI_PRODUCTISATION_EVIDENCE_ADOPTED: return 70U;
    case UMI_PRODUCTISATION_EVIDENCE_TESTED: return 85U;
    case UMI_PRODUCTISATION_EVIDENCE_ACCEPTED: return 100U;
    case UMI_PRODUCTISATION_EVIDENCE_DECLARED:
    case UMI_PRODUCTISATION_EVIDENCE_BLOCKED:
    default: return 0U;
    }
}

static void add_state(UmiProductApplicationCoverage *coverage,
                      UmiProductisationEvidenceState state,
                      uint64_t *weighted_total)
{
    coverage->asset_count += 1U;
    *weighted_total += evidence_weight(state);
    if (state == UMI_PRODUCTISATION_EVIDENCE_BLOCKED) {
        coverage->blocked_count += 1U;
        return;
    }
    if (state == UMI_PRODUCTISATION_EVIDENCE_DECLARED)
        coverage->unevidenced_count += 1U;
    if (state >= UMI_PRODUCTISATION_EVIDENCE_FOUNDATION)
        coverage->foundation_count += 1U;
    if (state >= UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED)
        coverage->implemented_count += 1U;
    if (state >= UMI_PRODUCTISATION_EVIDENCE_ADOPTED)
        coverage->adopted_count += 1U;
    if (state >= UMI_PRODUCTISATION_EVIDENCE_TESTED)
        coverage->tested_count += 1U;
    if (state >= UMI_PRODUCTISATION_EVIDENCE_ACCEPTED)
        coverage->accepted_count += 1U;
}

UmiStatus umi_product_portfolio_coverage_build(
    const UmiProductisationEvidenceLedger *ledger,
    UmiProductPortfolioCoverage *out_coverage)
{
    size_t application_index;
    uint64_t portfolio_weight = 0U;
    if (out_coverage == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_coverage, 0, sizeof(*out_coverage));

    for (application_index = 0U;
         application_index < umi_application_experience_catalogue_count();
         ++application_index) {
        const UmiApplicationExperienceDefinition *definition =
            umi_application_experience_catalogue_at(application_index);
        UmiProductApplicationCoverage *coverage;
        uint64_t application_weight = 0U;
        size_t index;
        if (definition == NULL ||
            out_coverage->application_count >=
                UMI_PRODUCTISATION_MAX_APPLICATIONS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        coverage = &out_coverage->applications[
            out_coverage->application_count++];
        (void)snprintf(coverage->application_id,
                       sizeof(coverage->application_id), "%s",
                       definition->application_id);

        for (index = 0U; index < definition->feature_count; ++index) {
            const UmiExperienceFeatureDefinition *feature =
                &definition->features[index];
            add_state(coverage,
                umi_productisation_evidence_effective_state(
                    ledger, definition->application_id,
                    UMI_PRODUCTISATION_ASSET_FEATURE, feature->feature_id,
                    feature_fallback(feature->state)),
                &application_weight);
        }
        for (index = 0U; index < definition->panel_count; ++index) {
            add_state(coverage,
                umi_productisation_evidence_effective_state(
                    ledger, definition->application_id,
                    UMI_PRODUCTISATION_ASSET_PANEL,
                    definition->panels[index].panel_id,
                    UMI_PRODUCTISATION_EVIDENCE_DECLARED),
                &application_weight);
        }
        for (index = 0U; index < definition->layout_count; ++index) {
            add_state(coverage,
                umi_productisation_evidence_effective_state(
                    ledger, definition->application_id,
                    UMI_PRODUCTISATION_ASSET_LAYOUT,
                    definition->layouts[index].layout_id,
                    UMI_PRODUCTISATION_EVIDENCE_DECLARED),
                &application_weight);
        }
        if (coverage->asset_count > 0U)
            coverage->coverage_percent = (unsigned)(
                application_weight / (uint64_t)coverage->asset_count);
        portfolio_weight += application_weight;
        out_coverage->asset_count += coverage->asset_count;
        out_coverage->accepted_count += coverage->accepted_count;
        out_coverage->blocked_count += coverage->blocked_count;
    }
    if (out_coverage->asset_count > 0U)
        out_coverage->coverage_percent = (unsigned)(
            portfolio_weight / (uint64_t)out_coverage->asset_count);
    return UMI_STATUS_OK;
}

const UmiProductApplicationCoverage *umi_product_portfolio_coverage_find(
    const UmiProductPortfolioCoverage *coverage,
    const char *application_id)
{
    size_t index;
    if (coverage == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < coverage->application_count; ++index) {
        if (strcmp(coverage->applications[index].application_id,
                   application_id) == 0)
            return &coverage->applications[index];
    }
    return NULL;
}
