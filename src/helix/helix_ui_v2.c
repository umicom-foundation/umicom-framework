/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/helix_ui_v2.c
 * PURPOSE: Build toolkit-neutral Helix v2 operational views.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/helix_ui_v2.h"

#include <stdint.h>
#include <stdio.h>

static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus create_view(const char *view_id, const char *kind,
                             const char *title, UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(view_id, "umicom.helix-agent-runtime-v2",
                                      UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "view.kind", kind);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_helix_ui_v2_overview(const char *view_id,
                                   const UmiHelixOrchestratorV2 *runtime,
                                   UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-overview", "Helix Agent Runtime v2",
                         out_view);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "helix.state", runtime->operation_ready
            ? umi_helix_operation_state_v2_text(runtime->operation.state) : "idle");
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "helix.agents", (int64_t)runtime->agents.count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "helix.journal-entries", (int64_t)runtime->journal.count);
    return status;
}

UmiStatus umi_helix_ui_v2_agents(const char *view_id,
                                 const UmiHelixOrchestratorV2 *runtime,
                                 UmiUiViewModel **out_view)
{
    size_t index;
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-agents", "Helix Agents", out_view);
    for (index = 0U; status == UMI_STATUS_OK && index < runtime->agents.count;
         ++index) {
        char key[64];
        (void)snprintf(key, sizeof(key), "agents.row.%zu", index);
        status = set_string(*out_view, key, runtime->agents.agents[index].agent_id);
    }
    return status;
}

UmiStatus umi_helix_ui_v2_approvals(const char *view_id,
                                    const UmiHelixOrchestratorV2 *runtime,
                                    UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-approvals", "Human Approvals", out_view);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "approvals.count", (int64_t)runtime->approvals.count);
    return status;
}

UmiStatus umi_helix_ui_v2_journal(const char *view_id,
                                  const UmiHelixOrchestratorV2 *runtime,
                                  UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-journal", "Plan, Action and Evidence Journal",
                         out_view);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "journal.count", (int64_t)runtime->journal.count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "journal.valid", umi_helix_journal_v2_verify(&runtime->journal));
    return status;
}

UmiStatus umi_helix_ui_v2_candidates(const char *view_id,
                                     const UmiHelixOrchestratorV2 *runtime,
                                     UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-candidates", "Candidate Comparison",
                         out_view);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "candidates.count", (int64_t)runtime->candidates.count);
    return status;
}

UmiStatus umi_helix_ui_v2_rollback(const char *view_id,
                                   const UmiHelixOrchestratorV2 *runtime,
                                   UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = create_view(view_id, "helix-rollback", "Rollback", out_view);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "rollback.remaining", (int64_t)runtime->rollback.remaining);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "rollback.state", (int64_t)runtime->rollback.state);
    return status;
}
