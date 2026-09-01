/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_ui/coding_assistant_views.c
 *
 * PURPOSE:
 *   Build toolkit-neutral AI coding, repository context and patch review views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A patch view is a preview and status surface.  Its action identifiers still
 * route through the host command registry, where user permissions and the
 * controlled filesystem adapter are enforced.
 */
#include "umicom/ai_ui/coding_assistant_views.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

#define UMI_AI_CODING_UI_VISIBLE_ROWS 16U

static UmiStatus set_string(UmiUiViewModel *view,
                            const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_boolean(UmiUiViewModel *view,
                             const char *key,
                             int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_action(UmiUiViewModel *view,
                            size_t index,
                            const char *action_id,
                            const char *label,
                            const char *tooltip)
{
    UmiUiCommandViewAction action = {0};
    (void)snprintf(action.action_id, sizeof(action.action_id), "%s", action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = 1;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus base_view(const char *view_id,
                           const char *kind,
                           const char *title,
                           const char *summary,
                           UmiUiViewModel **out_view)
{
    UmiStatus status;
    if (view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(
        view_id, "umicom.ai-coding-ui", UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "umicom.view-kind", kind);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

UmiStatus umi_ai_ui_coding_assistant_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    UmiUiViewModel **out_view)
{
    UmiAiCodingAssistantSnapshot snapshot;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-coding-assistant", "AI Coding Assistant",
        "Code chat, completion, explanation, refactoring and test generation with reviewed patches.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_assistant_snapshot(service, &snapshot);
    }
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.repository-files", (int64_t)snapshot.repository_files);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.tasks", (int64_t)snapshot.task_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.patches", (int64_t)snapshot.patch_count);
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.patches-awaiting-review",
        (int64_t)snapshot.draft_patches);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding.last-request", snapshot.last_request_id);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding.last-patch", snapshot.last_patch_id);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-coding.approval-required",
        snapshot.patch_policy.require_approval);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-coding.file-creation-allowed",
        snapshot.patch_policy.allow_create);
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-coding.file-deletion-allowed",
        snapshot.patch_policy.allow_delete);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.code-chat", "Code Chat",
        "Discuss the active code and repository context");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.complete-code", "Complete Code",
        "Plan a completion for the active cursor or selection");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.ai.explain-code", "Explain Code",
        "Explain the selected code with repository context");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.ai.refactor-code", "Refactor",
        "Prepare a reviewable refactoring patch");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.ai.generate-tests", "Generate Tests",
        "Prepare tests through a reviewable patch");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.pane.ai-coding-context", "Repository Context",
        "Inspect repository files eligible for coding prompts");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 6U, "studio.action.pane.ai-patch-review", "Patch Review",
        "Inspect the latest proposed coding patch");
    return status;
}

UmiStatus umi_ai_ui_coding_repository_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    UmiUiViewModel **out_view)
{
    UmiAiCodingContextIndex *context;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-coding-repository", "AI Repository Context",
        "Prioritised repository files with language, classification and token estimates.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    context = umi_ai_coding_assistant_context(service);
    count = umi_ai_coding_context_count(context);
    if (count > UMI_AI_CODING_UI_VISIBLE_ROWS) {
        count = UMI_AI_CODING_UI_VISIBLE_ROWS;
    }
    status = set_integer(*out_view, "ai-coding-context.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiCodingContextFile file;
        char key[96];
        char text[512];
        status = umi_ai_coding_context_at(context, index, &file);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "ai-coding-context.row.%zu", index);
        (void)snprintf(
            text, sizeof(text), "%.260s | %.48s | %" PRIu32
            " tokens | %.24s | %.16s",
            file.path, file.language_id, file.estimated_tokens,
            umi_ai_data_classification_text(file.classification),
            file.active ? "active" : (file.enabled ? "enabled" : "disabled"));
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.ai-coding", "Coding Assistant",
        "Return to the coding-assistant overview");
    return status;
}

UmiStatus umi_ai_ui_coding_patch_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    const char *patch_id,
    UmiUiViewModel **out_view)
{
    UmiAiCodingPatch patch;
    size_t count;
    size_t index;
    UmiStatus status;
    if (service == NULL || patch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-coding-patch", "AI Patch Review",
        "Exact before/after hashes, changed-line counts and controlled apply or revert state.",
        out_view);
    if (status != UMI_STATUS_OK) return status;
    if (patch_id[0] == '\0') {
        status = set_string(*out_view, "ai-coding-patch.state", "no patch");
        if (status == UMI_STATUS_OK) status = set_integer(
            *out_view, "ai-coding-patch.row-count", 0);
        return status;
    }
    status = umi_ai_coding_assistant_find_patch(service, patch_id, &patch);
    if (status != UMI_STATUS_OK) return status;
    status = set_string(*out_view, "ai-coding-patch.id", patch.patch_id);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.request", patch.request_id);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.title", patch.title);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.rationale", patch.rationale);
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.state",
        umi_ai_coding_patch_state_text(patch.state));
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding-patch.changed-lines", patch.changed_lines);
    count = patch.file_count;
    if (count > UMI_AI_CODING_UI_VISIBLE_ROWS) count = UMI_AI_CODING_UI_VISIBLE_ROWS;
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding-patch.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        const UmiAiCodingPatchFile *file = &patch.files[index];
        char key[96];
        char text[512];
        (void)snprintf(key, sizeof(key), "ai-coding-patch.row.%zu", index);
        (void)snprintf(
            text, sizeof(text), "%.260s | %.12s | +%" PRIu32 " -%" PRIu32
            " | %016" PRIx64 " -> %016" PRIx64,
            file->path, umi_ai_coding_patch_operation_text(file->operation),
            file->added_lines, file->removed_lines,
            file->before_hash, file->after_hash);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.patch-approve", "Approve",
        "Approve the exact reviewed patch revision");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.patch-apply", "Apply",
        "Apply the approved patch after workspace conflict checks");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.ai.patch-revert", "Revert",
        "Revert an unchanged applied patch");
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.pane.ai-coding", "Coding Assistant",
        "Return to the coding-assistant overview");
    return status;
}
