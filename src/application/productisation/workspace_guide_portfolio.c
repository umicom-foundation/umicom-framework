/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/workspace_guide_portfolio.c
 *
 * PURPOSE:
 *   Build and validate a bounded suite-level index of application workspace
 *   guidance without copying full layouts into a launcher.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/workspace_guide_portfolio.h"

#include <string.h>

/* Copy an exact identifier or title into memory owned by the portfolio. */
static UmiStatus portfolio_copy_text(char *destination,
                                     size_t capacity,
                                     const char *source)
{
    size_t length;

    /* All summary fields require valid storage and canonical source text. */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Reject truncation because application and layout IDs are persistent keys. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Copy the full guide fields needed by a compact launcher summary. */
static UmiStatus portfolio_copy_summary(
    const UmiProductWorkspaceGuide *guide,
    UmiProductWorkspaceGuideSummary *out_summary)
{
    UmiStatus status;

    /* Internal callers still receive a clear status when passed invalid storage. */
    if (guide == NULL || out_summary == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = portfolio_copy_text(
        out_summary->application_id, sizeof(out_summary->application_id),
        guide->application_id);
    /* Stop when a canonical application ID cannot be represented exactly. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = portfolio_copy_text(
        out_summary->display_name, sizeof(out_summary->display_name),
        guide->display_name);
    /* A launcher must never show a silently truncated product name. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = portfolio_copy_text(
        out_summary->executable_id, sizeof(out_summary->executable_id),
        guide->executable_id);
    /* Exact executable identity is required before a separate launch command. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = portfolio_copy_text(
        out_summary->recommended_layout_id,
        sizeof(out_summary->recommended_layout_id),
        guide->recommended_layout_id);
    /* The recommended layout ID must remain compatible with saved workspaces. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Unfinished feature text is optional when an application is fully verified. */
    if (guide->has_next_feature) {
        status = portfolio_copy_text(
            out_summary->next_feature_id,
            sizeof(out_summary->next_feature_id), guide->next_feature_id);
        /* Do not publish only half of the roadmap recommendation. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
        status = portfolio_copy_text(
            out_summary->next_feature_title,
            sizeof(out_summary->next_feature_title),
            guide->next_feature_title);
        /* The readable feature title must be complete for learner-facing views. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    out_summary->layout_choice_count = guide->choice_count;
    out_summary->total_panel_placements = guide->total_panel_placements;
    out_summary->readiness_percent = guide->readiness_percent;
    out_summary->has_next_feature = guide->has_next_feature;
    out_summary->acceptance_ready = guide->acceptance_ready;
    return UMI_STATUS_OK;
}

/* Build one small portfolio row from each registered thin application. */
UmiStatus umi_product_workspace_guide_portfolio_build(
    const UmiProductAdoptionRegistry *registry,
    UmiProductWorkspaceGuidePortfolio *out_portfolio)
{
    size_t readiness_total = 0U;
    size_t index;

    /* Both the contribution registry and output snapshot are required. */
    if (registry == NULL || out_portfolio == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Reject corrupted registries before using their fixed pointer array. */
    if (registry->count > UMI_PRODUCTISATION_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(out_portfolio, 0, sizeof(*out_portfolio));
    out_portfolio->structure_size = (uint32_t)sizeof(*out_portfolio);

    /* Each registered adoption is independently validated through its guide. */
    for (index = 0U; index < registry->count; ++index) {
        const UmiProductApplicationAdoption *adoption =
            umi_product_adoption_registry_at(registry, index);
        UmiProductWorkspaceGuide guide;
        UmiProductWorkspaceGuideSummary *summary =
            &out_portfolio->applications[out_portfolio->application_count];
        UmiStatus status;

        /* A missing registry entry indicates damaged or manually forged state. */
        if (adoption == NULL) {
            return UMI_STATUS_INVALID_STATE;
        }

        status = umi_product_workspace_guide_build(adoption, &guide);
        /* Invalid application guidance prevents a misleading suite summary. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        status = portfolio_copy_summary(&guide, summary);
        /* Never retain a partly copied portfolio row. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        out_portfolio->application_count += 1U;
        out_portfolio->layout_choice_count += summary->layout_choice_count;
        out_portfolio->panel_placement_count +=
            summary->total_panel_placements;
        readiness_total += (size_t)summary->readiness_percent;
        /* Count unfinished and accepted applications independently. */
        if (summary->has_next_feature) {
            out_portfolio->applications_with_next_feature += 1U;
        }
        /* Acceptance proves runnable surfaces, not that every roadmap item is done. */
        if (summary->acceptance_ready) {
            out_portfolio->acceptance_ready_count += 1U;
        }
    }

    /* An empty registry has a useful, well-defined zero-percent summary. */
    if (out_portfolio->application_count > 0U) {
        out_portfolio->average_readiness_percent = (unsigned)(
            readiness_total / out_portfolio->application_count);
    }

    return umi_product_workspace_guide_portfolio_validate(out_portfolio);
}

/* Recalculate all aggregate values and reject duplicate application identities. */
UmiStatus umi_product_workspace_guide_portfolio_validate(
    const UmiProductWorkspaceGuidePortfolio *portfolio)
{
    size_t layout_choice_count = 0U;
    size_t panel_placement_count = 0U;
    size_t next_feature_count = 0U;
    size_t acceptance_ready_count = 0U;
    size_t readiness_total = 0U;
    size_t index;

    /* Structure size and count protect every later fixed-array access. */
    if (portfolio == NULL ||
        portfolio->structure_size != (uint32_t)sizeof(*portfolio) ||
        portfolio->application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Validate each summary and recalculate values rather than trusting totals. */
    for (index = 0U; index < portfolio->application_count; ++index) {
        const UmiProductWorkspaceGuideSummary *summary =
            &portfolio->applications[index];
        size_t earlier_index;

        /* A usable launcher row needs identities, choices and bounded readiness. */
        if (summary->application_id[0] == '\0' ||
            summary->display_name[0] == '\0' ||
            summary->executable_id[0] == '\0' ||
            summary->recommended_layout_id[0] == '\0' ||
            summary->layout_choice_count == 0U ||
            summary->total_panel_placements == 0U ||
            summary->readiness_percent > 100U ||
            (summary->has_next_feature != 0 &&
             summary->has_next_feature != 1) ||
            (summary->acceptance_ready != 0 &&
             summary->acceptance_ready != 1)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* A next-feature marker is valid only with both ID and readable title. */
        if (summary->has_next_feature &&
            (summary->next_feature_id[0] == '\0' ||
             summary->next_feature_title[0] == '\0')) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* The bounded earlier-row scan detects duplicate canonical identities. */
        for (earlier_index = 0U; earlier_index < index; ++earlier_index) {
            /* Duplicate rows could cause a launcher to start the wrong program. */
            if (strcmp(
                    portfolio->applications[earlier_index].application_id,
                    summary->application_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }

        layout_choice_count += summary->layout_choice_count;
        panel_placement_count += summary->total_panel_placements;
        readiness_total += (size_t)summary->readiness_percent;
        /* Count optional roadmap and acceptance evidence for aggregate checks. */
        if (summary->has_next_feature) {
            next_feature_count += 1U;
        }
        /* Acceptance is independent of the feature roadmap maturity percentage. */
        if (summary->acceptance_ready) {
            acceptance_ready_count += 1U;
        }
    }

    /* Stored totals must exactly describe the rows supplied to the caller. */
    if (layout_choice_count != portfolio->layout_choice_count ||
        panel_placement_count != portfolio->panel_placement_count ||
        next_feature_count != portfolio->applications_with_next_feature ||
        acceptance_ready_count != portfolio->acceptance_ready_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Recalculate the average with the same empty-portfolio rule as the builder. */
    if (portfolio->application_count == 0U) {
        return portfolio->average_readiness_percent == 0U
            ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
    }

    return portfolio->average_readiness_percent ==
            (unsigned)(readiness_total / portfolio->application_count)
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Return one row without permitting access beyond the fixed public array. */
const UmiProductWorkspaceGuideSummary *
umi_product_workspace_guide_portfolio_at(
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    size_t index)
{
    /* Invalid structures and out-of-range indexes never expose memory. */
    if (portfolio == NULL ||
        portfolio->structure_size != (uint32_t)sizeof(*portfolio) ||
        portfolio->application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS ||
        index >= portfolio->application_count) {
        return NULL;
    }

    return &portfolio->applications[index];
}

/* Locate one application summary by its exact canonical identity. */
const UmiProductWorkspaceGuideSummary *
umi_product_workspace_guide_portfolio_find(
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    const char *application_id)
{
    size_t index;

    /* Empty identities cannot select an application from a suite launcher. */
    if (portfolio == NULL ||
        portfolio->structure_size != (uint32_t)sizeof(*portfolio) ||
        application_id == NULL ||
        application_id[0] == '\0' ||
        portfolio->application_count > UMI_PRODUCTISATION_MAX_APPLICATIONS) {
        return NULL;
    }

    /* Search only validated bounds and return memory borrowed from the snapshot. */
    for (index = 0U; index < portfolio->application_count; ++index) {
        const UmiProductWorkspaceGuideSummary *summary =
            umi_product_workspace_guide_portfolio_at(portfolio, index);

        /* Exact matching prevents one product prefix from selecting another. */
        if (summary != NULL &&
            strcmp(summary->application_id, application_id) == 0) {
            return summary;
        }
    }

    return NULL;
}
