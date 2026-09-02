/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/helix_ui.c
 * PURPOSE: Build toolkit-neutral Helix v2 operational views.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/helix_ui.h"

#include <stdint.h>
#include <stdio.h>

/* Provide the set string operation used by this module and its client applications. */
static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set integer operation used by this module and its client applications. */
static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the create view operation used by this module and its client applications. */
static UmiStatus create_view(const char *view_id, const char *kind,
                             const char *title, UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(view_id, "umicom.helix-agent-runtime",
                                      UMI_UI_ROLE_PANE, out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "view.kind", kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/* Provide the helix ui overview operation used by this module and its client applications. */
UmiStatus umi_helix_ui_overview(const char *view_id,
                                   const UmiHelixOrchestrator *runtime,
                                   UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-overview", "Helix Agent Runtime v2",
                         out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "helix.state", runtime->operation_ready
            ? umi_helix_operation_state_text(runtime->operation.state) : "idle");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "helix.agents", (int64_t)runtime->agents.count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "helix.journal-entries", (int64_t)runtime->journal.count);
    return status;
}

/* Provide the helix ui agents operation used by this module and its client applications. */
UmiStatus umi_helix_ui_agents(const char *view_id,
                                 const UmiHelixOrchestrator *runtime,
                                 UmiUiViewModel **out_view)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-agents", "Helix Agents", out_view);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < runtime->agents.count;
         ++index) {
        char key[64];
        (void)snprintf(key, sizeof(key), "agents.row.%zu", index);
        status = set_string(*out_view, key, runtime->agents.agents[index].agent_id);
    }
    return status;
}

/*
 * Provide the helix ui approvals operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_ui_approvals(const char *view_id,
                                    const UmiHelixOrchestrator *runtime,
                                    UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-approvals", "Human Approvals", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "approvals.count", (int64_t)runtime->approvals.count);
    return status;
}

/* Provide the helix ui journal operation used by this module and its client applications. */
UmiStatus umi_helix_ui_journal(const char *view_id,
                                  const UmiHelixOrchestrator *runtime,
                                  UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-journal", "Plan, Action and Evidence Journal",
                         out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "journal.count", (int64_t)runtime->journal.count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "journal.valid", umi_helix_journal_verify(&runtime->journal));
    return status;
}

/*
 * Provide the helix ui candidates operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_ui_candidates(const char *view_id,
                                     const UmiHelixOrchestrator *runtime,
                                     UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-candidates", "Candidate Comparison",
                         out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "candidates.count", (int64_t)runtime->candidates.count);
    return status;
}

/* Provide the helix ui rollback operation used by this module and its client applications. */
UmiStatus umi_helix_ui_rollback(const char *view_id,
                                   const UmiHelixOrchestrator *runtime,
                                   UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-rollback", "Rollback", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "rollback.remaining", (int64_t)runtime->rollback.remaining);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "rollback.state", (int64_t)runtime->rollback.state);
    return status;
}
