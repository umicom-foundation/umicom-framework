/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/instruction_inspection.inc
 * PURPOSE: Request instruction references and pages only while the captured native stop remains current.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "instruction_fields_internal.h"
#include "instruction_target_internal.h"
#include "variable_inspection_private.h"
/* Require one service owner and settled native state before capturing workspace evidence. */
static UmiStatus InstructionCurrent(UmiDebugRuntimePlatform *platform, UmiDebugWorkspace *workspace,
                                    UmiDebugInstructionContext *out)
{
    if (platform == NULL || workspace == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiDebugWorkspaceVariableService(workspace) != platform->service)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugInstructionContext value = {0};
    UmiStatus status = UmiDebugRuntimeConnectionIdentityRead(platform, &value.connection);
    if (status != UMI_STATUS_OK)
        return status;
    if (!platform->capabilities.supports_disassemble_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (!platform->paused || platform->active_thread_id == 0U ||
        platform->active_thread_id > INT32_MAX || platform->active_frame_id > INT32_MAX)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeAdapterSnapshot adapter;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK)
        return status;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugWorkspaceSnapshot selected;
    status = umi_debug_workspace_snapshot(workspace, &selected);
    if (status != UMI_STATUS_OK)
        return status;
    char frame[32];
    snprintf(frame, sizeof frame, "%llu", (unsigned long long)platform->active_frame_id);
    if (strcmp(frame, selected.selected_frame_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    value.thread_id = (uint32_t)platform->active_thread_id;
    value.frame_id = (uint32_t)platform->active_frame_id;
    value.revision = platform->revision;
    *out = value;
    return UMI_STATUS_OK;
}
/* Connection identity and workspace generations both protect delayed page requests. */
static UmiStatus InstructionTargetCheck(UmiDebugRuntimePlatform *platform,
                                        UmiDebugWorkspace *workspace,
                                        const UmiDebugInstructionTarget *target)
{
    if (target == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugInstructionContext current;
    UmiStatus status = InstructionCurrent(platform, workspace, &current);
    if (status != UMI_STATUS_OK)
        return status;
    /* The reference itself is immutable captured data. Current stopped-state
     * comparison concerns the connection, revision, thread and selected frame. */
    memcpy(current.reference, target->context.reference, sizeof current.reference);
    if (!UmiDebugInstructionContextEqual(&current, &target->context))
        return UMI_STATUS_INVALID_STATE;
    UmiDebugSelectionSnapshot selected;
    status = UmiDebugSelectionValidate(workspace, target->selection, &selected);
    if (status != UMI_STATUS_OK)
        return status;
    if (selected.kind != UMI_DEBUG_SELECT_FRAME || !selected.thread.stopped ||
        selected.thread.native_id != current.thread_id ||
        strcmp(selected.thread.session_id, current.connection.session_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/* Fetch the selected frame by stack position without replacing the current stack registry. */
UmiStatus UmiDebugInstructionTargetCapture(UmiDebugRuntimePlatform *platform,
                                           UmiDebugWorkspace *workspace, uint32_t timeout_ms,
                                           UmiDebugInstructionTarget **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDebugInstructionContext context;
    UmiStatus status = InstructionCurrent(platform, workspace, &context);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDebugInstructionTarget *target = calloc(1U, sizeof *target);
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    if (target == NULL || response == NULL)
    {
        free(target);
        free(response);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    target->context = context;
    UmiDebugWorkspaceSnapshot view;
    status = umi_debug_workspace_snapshot(workspace, &view);
    if (status == UMI_STATUS_OK)
    {
        status = UMI_STATUS_NOT_FOUND;
        for (size_t i = 0U; i < view.visible_frame_count; ++i)
        {
            UmiDebugStackFrameSnapshot frame;
            UmiStatus read = umi_debug_workspace_frame_at(workspace, i, &frame);
            if (read != UMI_STATUS_OK)
            {
                status = read;
                break;
            }
            if (strcmp(frame.id, view.selected_frame_id) == 0)
            {
                status = UmiDebugSelectionCapture(workspace, UMI_DEBUG_SELECT_FRAME, i,
                                                  &target->selection);
                break;
            }
        }
    }
    UmiDebugSelectionSnapshot selected;
    uint64_t sequence = 0U;
    if (status == UMI_STATUS_OK)
        status = InstructionTargetCheck(platform, workspace, target);
    if (status == UMI_STATUS_OK)
        status = UmiDebugSelectionRead(target->selection, &selected);
    if (status == UMI_STATUS_OK && selected.frame.order < 0)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_request_stack_trace(
            platform->adapter, context.thread_id, (uint32_t)selected.frame.order, 1U, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = DebugInstructionReferenceDecode(response->json, context.frame_id,
                                                 target->context.reference,
                                                 sizeof target->context.reference);
    if (status == UMI_STATUS_OK)
        status = InstructionTargetCheck(platform, workspace, target);
    if (status == UMI_STATUS_OK)
    {
        *out = target;
        target = NULL;
    }
    UmiDebugInstructionTargetDestroy(target);
    free(response);
    return status;
}
/* Recheck ownership after the reply because adapter events can retire the captured stop. */
UmiStatus UmiDebugInstructionPageRead(UmiDebugRuntimePlatform *platform,
                                      UmiDebugWorkspace *workspace,
                                      const UmiDebugInstructionTarget *target, int64_t byte_offset,
                                      int64_t instruction_offset, uint32_t count,
                                      int resolve_symbols, uint32_t timeout_ms,
                                      UmiDebugInstructionPage *out)
{
    if (out == NULL || timeout_ms == 0U || count == 0U ||
        count > UMI_DEBUG_INSTRUCTION_PAGE_LIMIT ||
        (resolve_symbols != 0 && resolve_symbols != 1) ||
        byte_offset < -UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT ||
        byte_offset > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT ||
        instruction_offset < -UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT ||
        instruction_offset > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = InstructionTargetCheck(platform, workspace, target);
    if (status != UMI_STATUS_OK)
        return status;
    if (target->context.reference[0] == '\0')
        return UMI_STATUS_NOT_FOUND;
    UmiDebugInstructionPage *page = malloc(sizeof *page);
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    if (page == NULL || response == NULL)
    {
        free(page);
        free(response);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_disassemble(platform->adapter, target->context.reference,
                                                   byte_offset, instruction_offset, count,
                                                   resolve_symbols, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms,
                                                         response);
    if (status == UMI_STATUS_OK)
        status = UmiDebugInstructionPageDecode(response->json, count, page);
    /* A continued, exited, or invalidated event can precede the reply. Leave it
     * queued for the host and refuse to publish instructions against an old stop. */
    if (status == UMI_STATUS_OK)
        status = InstructionTargetCheck(platform, workspace, target);
    if (status == UMI_STATUS_OK)
        *out = *page;
    free(page);
    free(response);
    return status;
}
