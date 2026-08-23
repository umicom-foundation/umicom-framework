/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/approvals.c
 *
 * PURPOSE:
 *   Project pending and decided sensitive operations into an approval centre.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/approvals.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ai_developer_experience/action_ids.h"

UmiStatus umi_ai_developer_approvals_view_create(
    const char *view_id,
    const UmiAiDeveloperApprovalQueue *approvals,
    const char *active_approval_id,
    size_t visible_rows,
    UmiUiViewModel **out_view)
{
    size_t total;
    size_t count;
    size_t first;
    size_t index;
    int active_pending = 0;
    UmiStatus status;

    if (approvals == NULL || active_approval_id == NULL ||
        visible_rows == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (visible_rows > UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY) {
        visible_rows = UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY;
    }

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-approvals",
        "AI Approvals",
        "Explicit review of sensitive tool calls, source-control mutations, debug control and patches.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    total = umi_ai_developer_approval_queue_count(approvals);
    count = total > visible_rows ? visible_rows : total;
    first = total - count;

    status = umi_ai_developer_view_set_integer(
        *out_view, "ai-approvals.total-count", (int64_t)total);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-approvals.pending-count",
            (int64_t)umi_ai_developer_approval_queue_pending_count(approvals));
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view, "ai-approvals.row-count", (int64_t)count);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view, "ai-approvals.active-id", active_approval_id);

    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiDeveloperApprovalRequest request;
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];
        int active;

        status = umi_ai_developer_approval_queue_at(
            approvals, first + index, &request);
        if (status != UMI_STATUS_OK) break;

        active =
            active_approval_id[0] != '\0' &&
            strcmp(active_approval_id, request.approval_id) == 0;

        if (active &&
            request.state == UMI_AI_DEVELOPER_APPROVAL_PENDING) {
            active_pending = 1;
        }

        (void)snprintf(key, sizeof(key), "ai-approvals.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s%s | %s | %s | risk %s | %.240s",
            active ? "* " : "",
            request.approval_id,
            umi_ai_developer_approval_kind_text(request.kind),
            umi_ai_developer_approval_state_text(request.state),
            umi_ai_coding_tool_risk_text(request.risk),
            request.title);

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_approval_approve(),
            "Approve",
            "Approve the selected sensitive operation",
            active_pending);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_approval_reject(),
            "Reject",
            "Reject the selected sensitive operation",
            active_pending);
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_open_overview(),
            "Overview",
            "Return to AI Developer overview",
            1);

    return status;
}
