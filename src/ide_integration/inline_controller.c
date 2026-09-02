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

/* Provide the record active operation used by this module and its client applications. */
static UmiStatus record_active(UmiIdeInlineController *controller)
{
    return umi_ide_inline_history_record(
        controller->history,
        &controller->active);
}

/*
 * Initialise ide inline controller from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_inline_controller_init(
    UmiIdeInlineController *controller,
    const UmiIdeInlineExecutor *executor,
    const UmiIdeEditorEditAdapter *edits)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || executor == NULL || edits == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ide_editor_edit_adapter_validate(edits);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(controller, 0, sizeof(*controller));
    controller->executor = *executor;
    controller->edits = *edits;
    controller->next_sequence = 1U;
    controller->revision = 1U;

    status = umi_ide_inline_history_create(&controller->history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(controller, 0, sizeof(*controller));
    }
    return status;
}

/*
 * Provide the ide inline controller deinit operation used by this module and its client
 * applications.
 */
void umi_ide_inline_controller_deinit(UmiIdeInlineController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return;
    umi_ide_inline_history_destroy(controller->history);
    (void)memset(controller, 0, sizeof(*controller));
}

/*
 * Provide the ide inline controller request operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_request(
    UmiIdeInlineController *controller,
    const UmiIdeEditorSelection *context)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)record_active(controller);
    }

    return status;
}

/*
 * Provide the ide inline controller accept operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_accept(
    UmiIdeInlineController *controller)
{
    uint64_t revision = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (controller->active.state != UMI_IDE_INLINE_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = controller->edits.get_revision(
        controller->edits.user_data,
        controller->active.document_id,
        &revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this operation only while the related capability or state is available. */
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

/*
 * Provide the ide inline controller reject operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_reject(
    UmiIdeInlineController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (controller->active.state != UMI_IDE_INLINE_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    controller->active.state = UMI_IDE_INLINE_REJECTED;
    controller->active.status = UMI_STATUS_OK;
    (void)record_active(controller);
    controller->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ide inline controller cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_cancel(
    UmiIdeInlineController *controller)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Use the stable identifier comparison to choose the matching record or policy. */
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

/*
 * Provide the ide inline controller snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ide_inline_controller_snapshot(
    const UmiIdeInlineController *controller,
    UmiIdeInlineSuggestion *out_suggestion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (controller == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_suggestion = controller->active;
    return UMI_STATUS_OK;
}
