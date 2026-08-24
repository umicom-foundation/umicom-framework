/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/selection_router.c
 *
 * PURPOSE:
 *   Implement cross-domain selection projection into IDE command context.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/selection_router.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    if (out == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

static void begin_selection(
    UmiStudioRuntimeSelectionRouter *router,
    UmiStudioRuntimeSelectionKind kind)
{
    umi_studio_selection_state_clear(&router->state);
    router->state.current.kind = kind;
    router->state.current.revision = router->state.revision + 1U;
    router->state.revision += 1U;
}

void umi_studio_selection_router_init(
    UmiStudioRuntimeSelectionRouter *router)
{
    if (router == NULL) return;

    (void)memset(router, 0, sizeof(*router));
    umi_studio_selection_state_init(&router->state);
    umi_ide_command_context_init(&router->command_context);
    router->revision = 1U;
}

UmiStatus umi_studio_selection_router_problem(
    UmiStudioRuntimeSelectionRouter *router,
    size_t problem_index,
    const char *label)
{
    if (router == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_PROBLEM);
    router->state.current.index = problem_index;
    (void)copy_text(
        router->state.current.label,
        sizeof(router->state.current.label),
        label);
    router->command_context.problem_index = problem_index;
    router->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_selection_router_test(
    UmiStudioRuntimeSelectionRouter *router,
    const char *test_item_id,
    const char *label)
{
    UmiStatus status;

    if (router == NULL || test_item_id == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_TEST);

    status = copy_text(
        router->state.current.subject_id,
        sizeof(router->state.current.subject_id),
        test_item_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->state.current.label,
            sizeof(router->state.current.label),
            label);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->command_context.test_item_id,
            sizeof(router->command_context.test_item_id),
            test_item_id);
    }
    if (status == UMI_STATUS_OK) router->revision += 1U;
    return status;
}

UmiStatus umi_studio_selection_router_source_control(
    UmiStudioRuntimeSelectionRouter *router,
    const char *path,
    uint32_t line)
{
    UmiStatus status;

    if (router == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_SOURCE_CONTROL);

    status = copy_text(
        router->state.current.path,
        sizeof(router->state.current.path),
        path);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->command_context.source_control_path,
            sizeof(router->command_context.source_control_path),
            path);
    }
    if (status == UMI_STATUS_OK) {
        router->state.current.line = line;
        router->command_context.source_control_line = line;
        router->revision += 1U;
    }
    return status;
}

UmiStatus umi_studio_selection_router_symbol(
    UmiStudioRuntimeSelectionRouter *router,
    const char *symbol_id,
    const char *label)
{
    UmiStatus status;

    if (router == NULL || symbol_id == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_SYMBOL);

    status = copy_text(
        router->state.current.subject_id,
        sizeof(router->state.current.subject_id),
        symbol_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->state.current.label,
            sizeof(router->state.current.label),
            label);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->command_context.symbol_id,
            sizeof(router->command_context.symbol_id),
            symbol_id);
    }
    if (status == UMI_STATUS_OK) router->revision += 1U;
    return status;
}

UmiStatus umi_studio_selection_router_diagnostic(
    UmiStudioRuntimeSelectionRouter *router,
    size_t diagnostic_index,
    const char *label)
{
    if (router == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_DIAGNOSTIC);
    router->state.current.index = diagnostic_index;
    (void)copy_text(
        router->state.current.label,
        sizeof(router->state.current.label),
        label);
    router->command_context.diagnostic_index = diagnostic_index;
    router->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_selection_router_debug_frame(
    UmiStudioRuntimeSelectionRouter *router,
    uint64_t frame_id)
{
    int written;

    if (router == NULL || frame_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_DEBUG_FRAME);
    written = snprintf(
        router->state.current.subject_id,
        sizeof(router->state.current.subject_id),
        "frame.%llu",
        (unsigned long long)frame_id);
    if (written < 0 ||
        (size_t)written >= sizeof(router->state.current.subject_id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    router->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_selection_router_editor(
    UmiStudioRuntimeSelectionRouter *router,
    const UmiIdeEditorSelection *selection)
{
    UmiStatus status;

    if (router == NULL || selection == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_editor_selection_validate(selection);
    if (status != UMI_STATUS_OK) return status;

    begin_selection(router, UMI_STUDIO_SELECTION_EDITOR);

    status = copy_text(
        router->state.current.subject_id,
        sizeof(router->state.current.subject_id),
        selection->document_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->state.current.path,
            sizeof(router->state.current.path),
            selection->path);
    }
    if (status != UMI_STATUS_OK) return status;

    router->state.current.line = selection->start_line;
    router->state.current.column = selection->start_column;
    router->command_context.editor_selection = *selection;
    router->command_context.has_editor_selection = 1;
    router->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_selection_router_ai_approval(
    UmiStudioRuntimeSelectionRouter *router,
    const char *approval_id,
    const char *label)
{
    UmiStatus status;

    if (router == NULL || approval_id == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_AI_APPROVAL);

    status = copy_text(
        router->state.current.subject_id,
        sizeof(router->state.current.subject_id),
        approval_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->state.current.label,
            sizeof(router->state.current.label),
            label);
    }
    if (status == UMI_STATUS_OK) router->revision += 1U;
    return status;
}

UmiStatus umi_studio_selection_router_ai_patch_file(
    UmiStudioRuntimeSelectionRouter *router,
    const char *patch_id,
    size_t file_index,
    const char *path)
{
    UmiStatus status;

    if (router == NULL || patch_id == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    begin_selection(router, UMI_STUDIO_SELECTION_AI_PATCH_FILE);

    status = copy_text(
        router->state.current.subject_id,
        sizeof(router->state.current.subject_id),
        patch_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            router->state.current.path,
            sizeof(router->state.current.path),
            path);
    }
    if (status == UMI_STATUS_OK) {
        router->state.current.index = file_index;
        router->revision += 1U;
    }
    return status;
}

const UmiIdeCommandContext *umi_studio_selection_router_context(
    const UmiStudioRuntimeSelectionRouter *router)
{
    return router != NULL ? &router->command_context : NULL;
}
