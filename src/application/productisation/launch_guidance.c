/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/launch_guidance.c
 *
 * PURPOSE:
 *   Build a safe launch preview that joins existing runtime choices with
 *   existing workspace-guide summaries without starting external processes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/launch_guidance.h"

#include <string.h>

#include "umicom/base/text.h"

/* Explain one unavailable choice without pretending to know hidden policy data. */
static UmiStatus launch_guidance_explain_unavailable(
    UmiProductGuidedLaunchEntry *entry)
{
    /* The runtime state is trustworthy even when individual policy flags are private. */
    return umi_text_format(
        entry->explanation,
        sizeof(entry->explanation),
        "%s is unavailable. It must be installed, compatible, enabled and "
        "visible before it can be selected. Its current runtime state is %s.",
        entry->display_name,
        umi_application_runtime_state_text(entry->runtime_state));
}

/* Explain a usable application that has not joined the guidance portfolio yet. */
static UmiStatus launch_guidance_explain_missing_guide(
    UmiProductGuidedLaunchEntry *entry)
{
    const char *action_text = entry->running ? "activate" : "start";

    /* An unselected row must not sound as though execution has been approved. */
    if (!entry->selected) {
        return umi_text_format(
            entry->explanation,
            sizeof(entry->explanation),
            "%s is available but not selected, and its workspace guide is "
            "not registered.",
            entry->display_name);
    }

    /* Missing descriptive guidance is a warning, not permission to block launch. */
    return umi_text_format(
        entry->explanation,
        sizeof(entry->explanation),
        "%s is selected and ready to %s, but its workspace guide is not registered. "
        "The application may still run through the governed launcher.",
        entry->display_name,
        action_text);
}

/* Explain an eligible choice that the user has not included in this launch. */
static UmiStatus launch_guidance_explain_not_selected(
    UmiProductGuidedLaunchEntry *entry)
{
    /* The recommended layout helps a beginner understand what selecting means. */
    return umi_text_format(
        entry->explanation,
        sizeof(entry->explanation),
        "%s is available but not selected. Its recommended starting layout is %s.",
        entry->display_name,
        entry->recommended_layout_id);
}

/* Explain the action that execution would request for a selected application. */
static UmiStatus launch_guidance_explain_ready(
    UmiProductGuidedLaunchEntry *entry)
{
    const char *action_text = entry->running ? "activate" : "start";
    const char *readiness_text = entry->acceptance_ready
        ? "Its product acceptance evidence is ready."
        : "Its product acceptance evidence still needs attention.";

    /* A preview combines action, layout and honest readiness in one short sentence. */
    return umi_text_format(
        entry->explanation,
        sizeof(entry->explanation),
        "%s is ready to %s with the recommended %s layout. %s",
        entry->display_name,
        action_text,
        entry->recommended_layout_id,
        readiness_text);
}

/* Copy the optional workspace summary into the plan entry when one exists. */
static UmiStatus launch_guidance_apply_workspace_summary(
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    UmiProductGuidedLaunchEntry *entry)
{
    const UmiProductWorkspaceGuideSummary *summary =
        umi_product_workspace_guide_portfolio_find(
            portfolio, entry->application_id);
    UmiStatus status;

    /* A missing row is retained as visible warning evidence for the launcher. */
    if (summary == NULL) {
        entry->guidance_available = 0;
        return UMI_STATUS_OK;
    }

    status = umi_text_copy(
        entry->recommended_layout_id,
        sizeof(entry->recommended_layout_id),
        summary->recommended_layout_id);
    /* Do not retain a partly copied guide when its persistent ID does not fit. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    entry->readiness_percent = summary->readiness_percent;
    entry->guidance_available = 1;
    entry->acceptance_ready = summary->acceptance_ready;
    return UMI_STATUS_OK;
}

/* Resolve the visible state and message after runtime and guide data are joined. */
static UmiStatus launch_guidance_resolve_entry(
    UmiProductGuidedLaunchEntry *entry)
{
    /* Runtime eligibility is authoritative and takes precedence over guidance. */
    if (!entry->eligible) {
        entry->guidance_state = UMI_PRODUCT_LAUNCH_GUIDANCE_UNAVAILABLE;
        return launch_guidance_explain_unavailable(entry);
    }

    /* A missing guide is visible even though the existing launcher may proceed. */
    if (!entry->guidance_available) {
        entry->guidance_state = UMI_PRODUCT_LAUNCH_GUIDANCE_MISSING_GUIDE;
        return launch_guidance_explain_missing_guide(entry);
    }

    /* Unselected applications remain informative without becoming executable. */
    if (!entry->selected) {
        entry->guidance_state = UMI_PRODUCT_LAUNCH_GUIDANCE_NOT_SELECTED;
        return launch_guidance_explain_not_selected(entry);
    }

    /* A running process is activated; a stopped process is started. */
    entry->guidance_state = entry->running
        ? UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_ACTIVATE
        : UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_START;
    return launch_guidance_explain_ready(entry);
}

/* Build one owned entry from one existing launch-selection choice. */
static UmiStatus launch_guidance_build_entry(
    const UmiApplicationLaunchChoice *choice,
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    UmiProductGuidedLaunchEntry *out_entry)
{
    UmiStatus status;

    /* All inputs are required because the entry joins both source snapshots. */
    if (choice == NULL || portfolio == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_entry, 0, sizeof(*out_entry));
    status = umi_text_copy(
        out_entry->application_id,
        sizeof(out_entry->application_id),
        choice->application_id);
    /* A persistent application ID must be copied completely. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_text_copy(
        out_entry->display_name,
        sizeof(out_entry->display_name),
        choice->display_name);
    /* A user-facing application name must not be silently shortened. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    out_entry->resolved_action = choice->running
        ? UMI_APPLICATION_LAUNCH_ACTIVATE
        : UMI_APPLICATION_LAUNCH_START;
    out_entry->runtime_state = choice->state;
    out_entry->selected = choice->selected ? 1 : 0;
    out_entry->eligible = choice->eligible ? 1 : 0;
    out_entry->running = choice->running ? 1 : 0;
    out_entry->ready_to_execute =
        choice->selected && choice->eligible ? 1 : 0;
    out_entry->runtime_revision = choice->revision;

    status = launch_guidance_apply_workspace_summary(portfolio, out_entry);
    /* Stop before resolving presentation when guide data cannot be represented. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    return launch_guidance_resolve_entry(out_entry);
}

/* Build a non-mutating preview from the current user selection and guides. */
UmiStatus umi_product_guided_launch_plan_build(
    const UmiApplicationLaunchSelection *selection,
    const UmiProductWorkspaceGuidePortfolio *portfolio,
    UmiProductGuidedLaunchPlan *out_plan)
{
    UmiApplicationLaunchSelectionSnapshot selection_snapshot;
    UmiApplicationLaunchSelectionSnapshot final_selection_snapshot;
    size_t index;
    UmiStatus status;

    /* A plan cannot be built without both authoritative input snapshots. */
    if (selection == NULL || portfolio == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_product_workspace_guide_portfolio_validate(portfolio);
    /* Invalid guidance must be rejected before any row is copied. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_application_launch_selection_snapshot(
        selection, &selection_snapshot);
    /* An invalid or unavailable selection cannot produce a trustworthy preview. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* The fixed output array uses the same maximum as the launch selection. */
    if (selection_snapshot.choice_count >
        UMI_PRODUCT_GUIDED_LAUNCH_MAX_ENTRIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    out_plan->structure_size = (uint32_t)sizeof(*out_plan);
    out_plan->selection_revision = selection_snapshot.revision;

    /* Each choice becomes one stable entry without changing selection state. */
    for (index = 0U; index < selection_snapshot.choice_count; ++index) {
        UmiApplicationLaunchChoice choice;
        UmiProductGuidedLaunchEntry *entry = &out_plan->entries[index];

        status = umi_application_launch_selection_at(
            selection, index, &choice);
        /* A changing or damaged selection is reported rather than partly rendered. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        status = launch_guidance_build_entry(&choice, portfolio, entry);
        /* A partially built plan is never returned as valid UI state. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        out_plan->entry_count += 1U;
        /* Aggregate independent facts so an adapter needs no second scan. */
        if (entry->selected) {
            out_plan->selected_count += 1U;
        }
        /* Only selected and eligible entries are safe to send to the launcher. */
        if (entry->ready_to_execute) {
            out_plan->ready_to_execute_count += 1U;
            /* A running entry contributes an activation, not a second process. */
            if (entry->resolved_action == UMI_APPLICATION_LAUNCH_ACTIVATE) {
                out_plan->activate_count += 1U;
            } else {
                out_plan->start_count += 1U;
            }
        }
        /* Unavailable and missing-guide counts include unselected catalogue rows. */
        if (!entry->eligible) {
            out_plan->unavailable_count += 1U;
        }
        if (!entry->guidance_available) {
            out_plan->missing_guidance_count += 1U;
        }
        /* Selected guidance gaps and incomplete acceptance deserve visible warnings. */
        if (entry->selected &&
            (!entry->guidance_available || !entry->acceptance_ready)) {
            out_plan->guidance_warning_count += 1U;
        }
    }

    /* Execution is possible only when every selected row remains eligible. */
    out_plan->executable =
        out_plan->selected_count > 0U &&
        out_plan->selected_count == out_plan->ready_to_execute_count;

    status = umi_application_launch_selection_snapshot(
        selection, &final_selection_snapshot);
    /* A caller can retry when another thread changed the selection mid-preview. */
    if (status != UMI_STATUS_OK ||
        final_selection_snapshot.revision != selection_snapshot.revision) {
        return status != UMI_STATUS_OK ? status : UMI_STATUS_BUSY;
    }

    return umi_product_guided_launch_plan_validate(out_plan);
}

/* Validate bounds, entry state and aggregate counters before presentation. */
UmiStatus umi_product_guided_launch_plan_validate(
    const UmiProductGuidedLaunchPlan *plan)
{
    size_t selected_count = 0U;
    size_t ready_count = 0U;
    size_t start_count = 0U;
    size_t activate_count = 0U;
    size_t unavailable_count = 0U;
    size_t missing_guidance_count = 0U;
    size_t guidance_warning_count = 0U;
    size_t index;

    /* The structure marker and fixed-array bound protect every later access. */
    if (plan == NULL ||
        plan->structure_size != (uint32_t)sizeof(*plan) ||
        plan->entry_count > UMI_PRODUCT_GUIDED_LAUNCH_MAX_ENTRIES ||
        (plan->executable != 0 && plan->executable != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Recalculate the complete plan instead of trusting stored aggregate values. */
    for (index = 0U; index < plan->entry_count; ++index) {
        const UmiProductGuidedLaunchEntry *entry = &plan->entries[index];
        size_t earlier_index;

        /* Required text and Boolean fields must be valid before UI rendering. */
        if (entry->application_id[0] == '\0' ||
            entry->display_name[0] == '\0' ||
            entry->explanation[0] == '\0' ||
            entry->runtime_state < UMI_APPLICATION_RUNTIME_UNKNOWN ||
            entry->runtime_state > UMI_APPLICATION_RUNTIME_STOPPING ||
            entry->readiness_percent > 100U ||
            (entry->selected != 0 && entry->selected != 1) ||
            (entry->eligible != 0 && entry->eligible != 1) ||
            (entry->running != 0 && entry->running != 1) ||
            (entry->guidance_available != 0 &&
             entry->guidance_available != 1) ||
            (entry->acceptance_ready != 0 &&
             entry->acceptance_ready != 1) ||
            (entry->ready_to_execute != 0 &&
             entry->ready_to_execute != 1)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Guide-backed rows require a layout; missing guides must own no stale layout. */
        if ((entry->guidance_available &&
             entry->recommended_layout_id[0] == '\0') ||
            (!entry->guidance_available &&
             entry->recommended_layout_id[0] != '\0')) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* The derived execution flag must exactly follow selection and eligibility. */
        if (entry->ready_to_execute !=
            (entry->selected && entry->eligible ? 1 : 0)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* The action is derived solely from whether a process already runs. */
        if ((entry->running &&
             entry->resolved_action != UMI_APPLICATION_LAUNCH_ACTIVATE) ||
            (!entry->running &&
             entry->resolved_action != UMI_APPLICATION_LAUNCH_START)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* State precedence mirrors the builder: availability, guide, selection, action. */
        if ((!entry->eligible &&
             entry->guidance_state !=
                 UMI_PRODUCT_LAUNCH_GUIDANCE_UNAVAILABLE) ||
            (entry->eligible && !entry->guidance_available &&
             entry->guidance_state !=
                 UMI_PRODUCT_LAUNCH_GUIDANCE_MISSING_GUIDE) ||
            (entry->eligible && entry->guidance_available &&
             !entry->selected &&
             entry->guidance_state !=
                 UMI_PRODUCT_LAUNCH_GUIDANCE_NOT_SELECTED) ||
            (entry->eligible && entry->guidance_available &&
             entry->selected && !entry->running &&
             entry->guidance_state !=
                 UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_START) ||
            (entry->eligible && entry->guidance_available &&
             entry->selected && entry->running &&
             entry->guidance_state !=
                 UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_ACTIVATE)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }

        /* Stable application IDs must appear at most once in one launch plan. */
        for (earlier_index = 0U; earlier_index < index; ++earlier_index) {
            /* Duplicate entries would make selection and result matching ambiguous. */
            if (strcmp(
                    plan->entries[earlier_index].application_id,
                    entry->application_id) == 0) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }

        /* Recalculate each independent total using the same rules as the builder. */
        if (entry->selected) {
            selected_count += 1U;
        }
        if (entry->ready_to_execute) {
            ready_count += 1U;
            /* Only the two preview actions are valid for a ready selection. */
            if (entry->resolved_action == UMI_APPLICATION_LAUNCH_ACTIVATE) {
                activate_count += 1U;
            } else {
                start_count += 1U;
            }
        }
        if (!entry->eligible) {
            unavailable_count += 1U;
        }
        if (!entry->guidance_available) {
            missing_guidance_count += 1U;
        }
        if (entry->selected &&
            (!entry->guidance_available || !entry->acceptance_ready)) {
            guidance_warning_count += 1U;
        }
    }

    /* Stored aggregate values must exactly match the validated entry scan. */
    if (plan->selected_count != selected_count ||
        plan->ready_to_execute_count != ready_count ||
        plan->start_count != start_count ||
        plan->activate_count != activate_count ||
        plan->unavailable_count != unavailable_count ||
        plan->missing_guidance_count != missing_guidance_count ||
        plan->guidance_warning_count != guidance_warning_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* A true executable flag means at least one selection and no blocked row. */
    return plan->executable ==
            (selected_count > 0U && selected_count == ready_count ? 1 : 0)
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Return a borrowed entry when its index belongs to the validated plan. */
const UmiProductGuidedLaunchEntry *umi_product_guided_launch_plan_at(
    const UmiProductGuidedLaunchPlan *plan,
    size_t index)
{
    /* Public lookup never reads beyond an invalid structure or fixed array. */
    if (plan == NULL ||
        plan->structure_size != (uint32_t)sizeof(*plan) ||
        plan->entry_count > UMI_PRODUCT_GUIDED_LAUNCH_MAX_ENTRIES ||
        index >= plan->entry_count) {
        return NULL;
    }

    return &plan->entries[index];
}

/* Find a borrowed entry by the stable application identifier. */
const UmiProductGuidedLaunchEntry *umi_product_guided_launch_plan_find(
    const UmiProductGuidedLaunchPlan *plan,
    const char *application_id)
{
    size_t index;

    /* An empty or missing identifier cannot name a launch-plan entry. */
    if (plan == NULL ||
        plan->structure_size != (uint32_t)sizeof(*plan) ||
        plan->entry_count > UMI_PRODUCT_GUIDED_LAUNCH_MAX_ENTRIES ||
        application_id == NULL || application_id[0] == '\0') {
        return NULL;
    }

    /* Search only through the bounded public accessor. */
    for (index = 0U; index < plan->entry_count; ++index) {
        const UmiProductGuidedLaunchEntry *entry =
            umi_product_guided_launch_plan_at(plan, index);

        /* Exact identity matching prevents prefix collisions between products. */
        if (entry != NULL &&
            strcmp(entry->application_id, application_id) == 0) {
            return entry;
        }
    }

    return NULL;
}

/* Convert a guidance state into short text suitable for logs and tests. */
const char *umi_product_launch_guidance_state_text(
    UmiProductLaunchGuidanceState state)
{
    /* The switch has an explicit fallback for corrupted values from boundaries. */
    switch (state) {
    case UMI_PRODUCT_LAUNCH_GUIDANCE_NOT_SELECTED:
        return "not-selected";
    case UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_START:
        return "ready-to-start";
    case UMI_PRODUCT_LAUNCH_GUIDANCE_READY_TO_ACTIVATE:
        return "ready-to-activate";
    case UMI_PRODUCT_LAUNCH_GUIDANCE_UNAVAILABLE:
        return "unavailable";
    case UMI_PRODUCT_LAUNCH_GUIDANCE_MISSING_GUIDE:
        return "missing-guide";
    default:
        return "invalid";
    }
}
