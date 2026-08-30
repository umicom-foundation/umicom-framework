/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/inline_controller.c
 *
 * PURPOSE:
 *   Implement revision-checked inline AI suggestion request/accept/reject flow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/inline_controller.h"

#include <string.h>

static UmiStatus record_active(UmiIdeInlineController *controller)
{
    return umi_ide_inline_history_record(
        controller->history,
        &controller->active);
}

UmiStatus umi_ide_inline_controller_init(
    UmiIdeInlineController *controller,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits)
{
    UmiStatus status;

    if (controller == NULL || executor == NULL || edits == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_editor_edit_adapter_validate(edits);
    if (status != UMI_STATUS_OK) return status;

    (void)memset(controller, 0, sizeof(*controller));
    controller->executor = *executor;
    controller->edits = *edits;
    controller->next_sequence = 1U;
    controller->revision = 1U;

    status = umi_ide_inline_history_create(&controller->history);
    if (status != UMI_STATUS_OK) {
        (void)memset(controller, 0, sizeof(*controller));
    }
    return status;
}

void umi_ide_inline_controller_deinit(UmiIdeInlineController *controller)
{
    if (controller == NULL) return;
    umi_ide_inline_history_destroy(controller->history);
    (void)memset(controller, 0, sizeof(*controller));
}

UmiStatus umi_ide_inline_controller_request(
    UmiIdeInlineController *controller,
    const UmiIdeEditorSelection *context)
{
    UmiStatus status;

    if (controller == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (controller->active.state == UMI_IDE_INLINE_READY ||
        controller->active.state == UMI_IDE_INLINE_REQUESTED) {
        return UMI_STATUS_BUSY;
    }

    (void)memset(&controller->active, 0, sizeof(controller->active));
    controller->active.state = UMI_IDE_INLINE_REQUESTED;
    controller->revision += 1U;

    status = umi_ide_inline_execute(
        &controller->executor,
        context,
        controller->next_sequence++,
        &controller->active);

    controller->revision += 1U;

    if (status != UMI_STATUS_OK) {
        (void)record_active(controller);
    }

    return status;
}

UmiStatus umi_ide_inline_controller_accept(
    UmiIdeInlineController *controller)
{
    uint64_t revision = 0U;
    UmiStatus status;

    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (controller->active.state != UMI_IDE_INLINE_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = controller->edits.get_revision(
        controller->edits.user_data,
        controller->active.document_id,
        &revision);
    if (status != UMI_STATUS_OK) return status;

    if (revision != controller->active.document_revision) {
        controller->active.state = UMI_IDE_INLINE_FAILED;
        controller->active.status = UMI_STATUS_INVALID_STATE;
        (void)record_active(controller);
        controller->revision += 1U;
        return UMI_STATUS_INVALID_STATE;
    }

    status = controller->edits.insert_text(
        controller->edits.user_data,
        controller->active.document_id,
        controller->active.line,
        controller->active.column,
        controller->active.text,
        strlen(controller->active.text));

    controller->active.status = status;
    controller->active.state =
        status == UMI_STATUS_OK
            ? UMI_IDE_INLINE_ACCEPTED
            : UMI_IDE_INLINE_FAILED;
    (void)record_active(controller);
    controller->revision += 1U;
    return status;
}

UmiStatus umi_ide_inline_controller_reject(
    UmiIdeInlineController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (controller->active.state != UMI_IDE_INLINE_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    controller->active.state = UMI_IDE_INLINE_REJECTED;
    controller->active.status = UMI_STATUS_OK;
    (void)record_active(controller);
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_inline_controller_cancel(
    UmiIdeInlineController *controller)
{
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (controller->active.state != UMI_IDE_INLINE_REQUESTED &&
        controller->active.state != UMI_IDE_INLINE_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    controller->active.state = UMI_IDE_INLINE_CANCELLED;
    controller->active.status = UMI_STATUS_CANCELLED;
    (void)record_active(controller);
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ide_inline_controller_snapshot(
    const UmiIdeInlineController *controller,
    UmiIdeInlineSuggestion *out_suggestion)
{
    if (controller == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_suggestion = controller->active;
    return UMI_STATUS_OK;
}
