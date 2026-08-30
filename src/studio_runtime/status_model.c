/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/status_model.c
 *
 * PURPOSE:
 *   Implement authoritative IDE-context -> Studio status-bar projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/status_model.h"

#include <stdio.h>
#include <string.h>

static UmiStatus append(
    UmiStudioRuntimeStatusModel *model,
    UmiStudioRuntimeStatusKind kind,
    const char *item_id,
    const char *text,
    const char *command_id,
    uint32_t badge_count,
    int important)
{
    UmiStudioRuntimeStatusItem *item;
    size_t id_length;
    size_t text_length;
    size_t command_length;

    if (model->item_count >= UMI_STUDIO_RUNTIME_STATUS_ITEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    id_length = strlen(item_id);
    text_length = strlen(text);
    command_length = strlen(command_id);

    if (id_length >= UMI_STUDIO_RUNTIME_ID_CAPACITY ||
        text_length >= UMI_STUDIO_RUNTIME_TEXT_CAPACITY ||
        command_length >= UMI_STUDIO_RUNTIME_ID_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    item = &model->items[model->item_count++];
    (void)memset(item, 0, sizeof(*item));
    item->kind = kind;
    (void)memcpy(item->item_id, item_id, id_length + 1U);
    (void)memcpy(item->text, text, text_length + 1U);
    (void)memcpy(item->command_id, command_id, command_length + 1U);
    item->badge_count = badge_count;
    item->visible = 1;
    item->important = important != 0;
    item->revision = model->revision + 1U;
    return UMI_STATUS_OK;
}

void umi_studio_status_model_init(UmiStudioRuntimeStatusModel *model)
{
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->revision = 1U;
}

UmiStatus umi_studio_status_model_build(
    UmiStudioRuntimeStatusModel *model,
    const UmiIdeIntegrationPlatformSnapshot *snapshot,
    const UmiStudioRuntimeSelectionRouter *selection)
{
    char text[512];
    uint32_t problem_badge = 0U;
    UmiStatus status;

    if (model == NULL || snapshot == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    model->item_count = 0U;

    status = append(
        model,
        UMI_STUDIO_STATUS_WORKSPACE,
        "umicom.shell.status-bar.workspace",
        snapshot->context.workspace_root,
        "",
        0U,
        0);

    if (status == UMI_STATUS_OK) {
        status = append(
            model,
            UMI_STUDIO_STATUS_BRANCH,
            "umicom.shell.status-bar.branch",
            snapshot->context.has_source_control
                ? snapshot->context.source_control.branch.name
                : "No VCS",
            "view.source-control",
            snapshot->context.has_source_control
                ? (uint32_t)(snapshot->context.source_control.change_count >
                        UINT32_MAX
                    ? UINT32_MAX
                    : snapshot->context.source_control.change_count)
                : 0U,
            0);
    }

    if (snapshot->context.has_problems) {
        const size_t severe =
            snapshot->context.problems.errors +
            snapshot->context.problems.fatals;
        problem_badge =
            severe > UINT32_MAX ? UINT32_MAX : (uint32_t)severe;
    }

    if (status == UMI_STATUS_OK) {
        (void)snprintf(
            text,
            sizeof(text),
            "Errors %zu  Warnings %zu",
            snapshot->context.has_problems
                ? snapshot->context.problems.errors +
                  snapshot->context.problems.fatals
                : 0U,
            snapshot->context.has_problems
                ? snapshot->context.problems.warnings
                : 0U);

        status = append(
            model,
            UMI_STUDIO_STATUS_PROBLEMS,
            "umicom.shell.status-bar.problems",
            text,
            "view.problems",
            problem_badge,
            problem_badge > 0U);
    }

    if (status == UMI_STATUS_OK) {
        (void)snprintf(
            text,
            sizeof(text),
            "Tests %zu%s",
            snapshot->context.has_tests
                ? snapshot->context.tests.result_count
                : 0U,
            snapshot->context.has_tests &&
                    snapshot->context.tests.operation_running
                ? " running"
                : "");

        status = append(
            model,
            UMI_STUDIO_STATUS_TESTS,
            "umicom.studio.status.tests",
            text,
            "view.test-explorer",
            0U,
            snapshot->context.has_tests &&
                snapshot->context.tests.operation_running);
    }

    if (status == UMI_STATUS_OK) {
        status = append(
            model,
            UMI_STUDIO_STATUS_BUILD,
            "umicom.shell.status-bar.build",
            snapshot->workflow.ready ? "Ready" : "Workflow blocked",
            "view.build-dashboard",
            snapshot->workflow.blocked_count > UINT32_MAX
                ? UINT32_MAX
                : (uint32_t)snapshot->workflow.blocked_count,
            !snapshot->workflow.ready);
    }

    if (status == UMI_STATUS_OK) {
        (void)snprintf(
            text,
            sizeof(text),
            "%s%s",
            snapshot->context.has_debug
                ? (snapshot->context.debug.active ? "Debugging" : "Debugger idle")
                : "Debugger unavailable",
            snapshot->context.has_debug && snapshot->context.debug.paused
                ? " paused"
                : "");

        status = append(
            model,
            UMI_STUDIO_STATUS_DEBUG,
            "umicom.studio.status.debug",
            text,
            "view.debug",
            0U,
            snapshot->context.has_debug && snapshot->context.debug.active);
    }

    if (status == UMI_STATUS_OK) {
        status = append(
            model,
            UMI_STUDIO_STATUS_AI,
            "umicom.studio.status.ai",
            snapshot->context.has_ai
                ? umi_ai_coding_runtime_state_text(
                    snapshot->context.ai.coding.agent.state)
                : "AI unavailable",
            "ai.assistant.open",
            snapshot->context.has_ai
                ? (uint32_t)(snapshot->context.ai.pending_approval_count >
                        UINT32_MAX
                    ? UINT32_MAX
                    : snapshot->context.ai.pending_approval_count)
                : 0U,
            snapshot->context.has_ai &&
                snapshot->context.ai.pending_approval_count > 0U);
    }

    if (status == UMI_STATUS_OK) {
        status = append(
            model,
            UMI_STUDIO_STATUS_LANGUAGE,
            "umicom.shell.status-bar.language",
            snapshot->context.has_document
                ? snapshot->context.document.language_id
                : "",
            "",
            0U,
            0);
    }

    if (status == UMI_STATUS_OK) {
        status = append(
            model,
            UMI_STUDIO_STATUS_ENCODING,
            "umicom.shell.status-bar.encoding",
            snapshot->context.has_document
                ? umi_document_encoding_text(
                    snapshot->context.document.encoding)
                : "",
            "",
            0U,
            0);
    }

    if (status == UMI_STATUS_OK) {
        (void)snprintf(
            text,
            sizeof(text),
            "Ln %u, Col %u",
            selection->state.current.line,
            selection->state.current.column);

        status = append(
            model,
            UMI_STUDIO_STATUS_CURSOR,
            "umicom.shell.status-bar.line-column",
            text,
            "",
            0U,
            0);
    }

    if (status == UMI_STATUS_OK) model->revision += 1U;
    return status;
}

const UmiStudioRuntimeStatusItem *umi_studio_status_model_find(
    const UmiStudioRuntimeStatusModel *model,
    UmiStudioRuntimeStatusKind kind)
{
    size_t index;

    if (model == NULL) return NULL;

    for (index = 0U; index < model->item_count; ++index) {
        if (model->items[index].kind == kind) return &model->items[index];
    }

    return NULL;
}
