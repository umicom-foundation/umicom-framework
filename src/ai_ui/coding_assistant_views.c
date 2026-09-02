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

/* Provide the set string operation used by this module and its client applications. */
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

/* Provide the set integer operation used by this module and its client applications. */
static UmiStatus set_integer(UmiUiViewModel *view,
                             const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set boolean operation used by this module and its client applications. */
static UmiStatus set_boolean(UmiUiViewModel *view,
                             const char *key,
                             int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set action operation used by this module and its client applications. */
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

/* Provide the base view operation used by this module and its client applications. */
static UmiStatus base_view(const char *view_id,
                           const char *kind,
                           const char *title,
                           const char *summary,
                           UmiUiViewModel **out_view)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id == NULL || out_view == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_view = NULL;
    status = umi_ui_view_model_create(
        view_id, "umicom.ai-coding-ui", UMI_UI_ROLE_PANE, out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "umicom.view-kind", kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "summary", summary);
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

/*
 * Initialise ai ui coding assistant view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_ui_coding_assistant_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    UmiUiViewModel **out_view)
{
    UmiAiCodingAssistantSnapshot snapshot;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-coding-assistant", "AI Coding Assistant",
        "Code chat, completion, explanation, refactoring and test generation with reviewed patches.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_assistant_snapshot(service, &snapshot);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.repository-files", (int64_t)snapshot.repository_files);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.tasks", (int64_t)snapshot.task_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.patches", (int64_t)snapshot.patch_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding.patches-awaiting-review",
        (int64_t)snapshot.draft_patches);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding.last-request", snapshot.last_request_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding.last-patch", snapshot.last_patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-coding.approval-required",
        snapshot.patch_policy.require_approval);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-coding.file-creation-allowed",
        snapshot.patch_policy.allow_create);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_boolean(
        *out_view, "ai-coding.file-deletion-allowed",
        snapshot.patch_policy.allow_delete);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.code-chat", "Code Chat",
        "Discuss the active code and repository context");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.complete-code", "Complete Code",
        "Plan a completion for the active cursor or selection");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.ai.explain-code", "Explain Code",
        "Explain the selected code with repository context");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.ai.refactor-code", "Refactor",
        "Prepare a reviewable refactoring patch");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.ai.generate-tests", "Generate Tests",
        "Prepare tests through a reviewable patch");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.pane.ai-coding-context", "Repository Context",
        "Inspect repository files eligible for coding prompts");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 6U, "studio.action.pane.ai-patch-review", "Patch Review",
        "Inspect the latest proposed coding patch");
    return status;
}

/*
 * Initialise ai ui coding repository view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_ui_coding_repository_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *service,
    UmiUiViewModel **out_view)
{
    UmiAiCodingContextIndex *context;
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-coding-repository", "AI Repository Context",
        "Prioritised repository files with language, classification and token estimates.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    context = umi_ai_coding_assistant_context(service);
    count = umi_ai_coding_context_count(context);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_AI_CODING_UI_VISIBLE_ROWS) {
        count = UMI_AI_CODING_UI_VISIBLE_ROWS;
    }
    status = set_integer(*out_view, "ai-coding-context.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiAiCodingContextFile file;
        char key[96];
        char text[512];
        status = umi_ai_coding_context_at(context, index, &file);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.pane.ai-coding", "Coding Assistant",
        "Return to the coding-assistant overview");
    return status;
}

/*
 * Initialise ai ui coding patch view from caller-provided values so later operations
 * receive a known state.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || patch_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = base_view(
        view_id, "ai-coding-patch", "AI Patch Review",
        "Exact before/after hashes, changed-line counts and controlled apply or revert state.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (patch_id[0] == '\0') {
        status = set_string(*out_view, "ai-coding-patch.state", "no patch");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = set_integer(
            *out_view, "ai-coding-patch.row-count", 0);
        return status;
    }
    status = umi_ai_coding_assistant_find_patch(service, patch_id, &patch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = set_string(*out_view, "ai-coding-patch.id", patch.patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.request", patch.request_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.title", patch.title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.rationale", patch.rationale);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(
        *out_view, "ai-coding-patch.state",
        umi_ai_coding_patch_state_text(patch.state));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding-patch.changed-lines", patch.changed_lines);
    count = patch.file_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_AI_CODING_UI_VISIBLE_ROWS) count = UMI_AI_CODING_UI_VISIBLE_ROWS;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_integer(
        *out_view, "ai-coding-patch.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.ai.patch-approve", "Approve",
        "Approve the exact reviewed patch revision");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.ai.patch-apply", "Apply",
        "Apply the approved patch after workspace conflict checks");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.ai.patch-revert", "Revert",
        "Revert an unchanged applied patch");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.pane.ai-coding", "Coding Assistant",
        "Return to the coding-assistant overview");
    return status;
}
